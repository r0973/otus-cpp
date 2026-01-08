#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <string>
#include <functional>
#include <memory>
#include <typeindex>
#include <vector>
#include "AnyData.h"
#include "LRUCache.h"
#include "SecondaryIndex.h"
#include "StorageConfig.h"
#include "WalManager.h"
#include "Logger.h"

namespace nosqldb
{

class MemoryStorage {
private:
    // Основное хранилище
    std::unordered_map<std::string, AnyData> mainStore_;
    mutable std::shared_mutex storeMutex_;
    
    // LRU кэш
    LRUCache<std::string, AnyData> lruCache_;
    
    // Вторичные индексы
    SecondaryIndexManager indexManager_;
    bool enableIndexing_;
    
    // Extractors для индексов
    struct ExtractorInfo {
        std::type_index dataType;
        std::type_index fieldType;
        std::shared_ptr<void> extractorPtr; // type-erased extractor
    };
    
    std::unordered_map<std::string, ExtractorInfo> extractors_;
    mutable std::shared_mutex extractorMutex_;
    
    // Версионность
    std::unordered_map<std::string, std::vector<AnyData>> versionStore_;
    mutable std::shared_mutex versionMutex_;
    bool enableVersioning_;
    size_t maxVersions_;

protected:
    // WAL
    std::unique_ptr<WalManager> wal_manager_;
    bool enable_wal_ = false;

protected:
    void EnableWal(const std::string& path) {
        wal_manager_ = std::make_unique<WalManager>(path);
    }

public:
    explicit MemoryStorage(const StorageConfig& config = StorageConfig{})
        : mainStore_{},
          lruCache_{config.lruCacheCapacity},
          indexManager_{},
          enableIndexing_(config.enableIndexing),
          extractors_{},
          versionStore_{},
          enableVersioning_{config.enableVersioning},
          maxVersions_(config.maxVersionsPerKey),
          wal_manager_{nullptr},
          enable_wal_{config.enableWAL}
    {
        if (enable_wal_) {
            wal_manager_ = std::make_unique<WalManager>(config.dataDirectory);
        }
    }

public:
    virtual ~MemoryStorage() = default; 

    // 2. Виртуальный метод для сохранения
    virtual void Save() {
        // Базовый класс ничего не делает
    }

    // Восстановление из WAL
    void RecoverFromWAL(const std::string& dataDirectory) {
        if (!enable_wal_)
            return;
        
        if (!wal_manager_) {
            wal_manager_ = std::make_unique<WalManager>(dataDirectory);
        }

        wal_manager_->Recover(
            // Обработчик для Put (type 1)
            [this](const std::string& key, const AnyData& value) {
                std::unique_lock lock(this->storeMutex_);
                this->mainStore_[key] = value;
            },
            // Обработчик для Delete (type 2)
            [this](const std::string& key) {
                std::unique_lock lock(this->storeMutex_);
                this->mainStore_.erase(key);
            }
        );
        
        RebuildIndices();
    }

    // Основные операции
    template<typename T>
    bool Put(const std::string& key, const T& value) {
        std::unique_lock lock(storeMutex_);

        // Сначала записываем в WAL (если включен)
        if (enable_wal_ && wal_manager_) {
            auto proto = AnyData(value).ToProto();
            wal_manager_->LogPut(key, proto);
        }

        // Получаем старое значение для обновления индексов
        std::optional<T> oldValue;
        auto it = mainStore_.find(key);
        if (it != mainStore_.end()) {
            T val;
            if (it->second.TryGet(val)) {
                oldValue = val;
            }
        }
        
        // Сохраняем в основном хранилище
        mainStore_[key] = AnyData(value);
        
        // Обновляем версионность если включена
        if (enableVersioning_) {
            AddVersion(key, value);
        }

        // Обновляем LRU кэш
        lruCache_.Put(key, AnyData(value));
                
        // Обновляем индексы
        if (enableIndexing_) {
            if (oldValue.has_value()) {
                UpdateIndices<T>(key, oldValue.value(), value);
            } else {
                AddToIndices<T>(key, value);
            }
        }
        
        return true;
    }
    
    std::optional<AnyData> GetAnyData(const std::string& key) {
        // 1. Сначала пробуем получить из кэша
        auto cached = lruCache_.Get(key);
        if (cached.has_value()) {
            return cached; // Возвращаем AnyData из кэша
        }

        // 2. Если в кэше нет, ищем в основном хранилище
        std::shared_lock lock(storeMutex_);
        
        auto it = mainStore_.find(key);
        if (it != mainStore_.end()) {
            // Ключ найден в памяти
            AnyData value = it->second;
            
            // 3. Обновляем кэш, чтобы при следующем запросе взять оттуда
            lruCache_.Put(key, value);
            
            return value;
        }

        return std::nullopt;
    }

    template<typename T>
    std::optional<T> Get(const std::string& key) {
        auto data = GetAnyData(key);
        if (data.has_value()) {
            T value;
            if (data->TryGet(value)) {
                return value;
            }
        }
        return std::nullopt;
    }
    
    bool Delete(const std::string& key) {
        std::unique_lock lock(storeMutex_);
        
        // Сначала записываем в WAL
        if (enable_wal_ && wal_manager_) {
            wal_manager_->LogDelete(key);
        }

        auto it = mainStore_.find(key);
        if (it == mainStore_.end()) {
            return false;
        }
        
        // Получаем значение для удаления из индексов
        AnyData value = it->second;
        
        // Удаляем из основного хранилища
        mainStore_.erase(it);
        
        // Удаляем из LRU кэша
        lruCache_.Remove(key);
        
        // Удаляем версии если включено
        if (enableVersioning_) {
            std::unique_lock vlock(versionMutex_); 
            versionStore_.erase(key);
        }
        
        // Удаляем из индексов
        if (enableIndexing_) {
            RemoveFromIndices(key, value);
        }
        
        return true;
    }
    
    bool Exists(const std::string& key) const {
        // Сначала проверяем кэш
        if (lruCache_.Contains(key)) {
            return true;
        }
        
        std::shared_lock lock(storeMutex_);
        return mainStore_.find(key) != mainStore_.end();
    }
    
    size_t Size() const {
        std::shared_lock lock(storeMutex_);
        return mainStore_.size();
    }
    
    void Clear() {
        std::unique_lock lock(storeMutex_);
        std::unique_lock vlock(versionMutex_);
        
        mainStore_.clear();
        versionStore_.clear();
        
        lruCache_.Clear();
        if (enableIndexing_) {
            indexManager_.ClearAll();
            extractors_.clear();
        }
    }
    
    std::vector<std::string> Keys() const {
        std::shared_lock lock(storeMutex_);
        std::vector<std::string> keys;
        keys.reserve(mainStore_.size());
        
        for (const auto& [key, _] : mainStore_) {
            keys.push_back(key);
        }
        
        return keys;
    }
    
    // Вторичные индексы
    template<typename DataType, typename FieldType>
    void CreateIndex(const std::string& indexName,
                    std::function<FieldType(const DataType&)> extractor) {
        if (!enableIndexing_) {
            throw std::runtime_error("Indexing is disabled");
        }
        
        std::unique_lock lock(extractorMutex_);
        
        // Сохраняем extractor
        extractors_.emplace(indexName, ExtractorInfo{
            typeid(DataType),
            typeid(FieldType),
            std::make_shared<std::function<FieldType(const DataType&)>>(extractor)
        });
        
        // Создаем индекс
        indexManager_.CreateIndex<FieldType>(indexName);
        
        // Строим индекс по существующим данным
        RebuildIndex<DataType, FieldType>(indexName, extractor);
    }
    
    template<typename FieldType>
    std::vector<std::string> QueryByIndex(const std::string& indexName,
                                         const FieldType& value) {
        if (!enableIndexing_) {
            throw std::runtime_error("Indexing is disabled");
        }
        
        auto index = indexManager_.GetIndex<FieldType>(indexName);
        if (!index) {
            return {};
        }
        
        auto keys = index->Find(value);
        return std::vector<std::string>(keys.begin(), keys.end());
    }
    
    template<typename FieldType>
    std::vector<std::string> QueryRange(const std::string& indexName,
                                       const FieldType& minValue,
                                       const FieldType& maxValue) {
        if (!enableIndexing_) {
            throw std::runtime_error("Indexing is disabled");
        }
        
        auto index = indexManager_.GetIndex<FieldType>(indexName);
        if (!index) {
            return {};
        }
        
        auto keys = index->FindRange(minValue, maxValue);
        return std::vector<std::string>(keys.begin(), keys.end());
    }
    
    // Версионность
    template<typename T>
    bool PutWithVersion(const std::string& key, const T& value) {
        if (!enableVersioning_) {
            return Put(key, value);
        }
        
        return Put(key, value); // Версионность уже обрабатывается в Put
    }
    
    template<typename T>
    std::vector<T> GetVersions(const std::string& key, size_t limit = 10) {
        if (!enableVersioning_) {
            auto value = Get<T>(key);
            if (value.has_value()) {
                return {value.value()};
            }
            return {};
        }
        
        std::shared_lock lock(versionMutex_);
        auto it = versionStore_.find(key);
        if (it == versionStore_.end()) {
            return {};
        }
        
        std::vector<T> result;
        size_t count = 0;
        
        // Возвращаем от новых к старым
        for (auto rit = it->second.rbegin(); 
             rit != it->second.rend() && count < limit; 
             ++rit, ++count) {
            T value;
            if (rit->TryGet(value)) {
                result.push_back(value);
            }
        }
        
        return result;
    }
    
    // Метрики
    struct Metrics {
        size_t totalItems = 0;
        size_t cachedItems = 0;
        size_t indexCount = 0;
        double cacheHitRatio = 0.0;
        size_t versionCount = 0;
    };
    
    Metrics GetMetrics() const {
        Metrics metrics;
        
        std::shared_lock lock(storeMutex_);
        metrics.totalItems = mainStore_.size();
        lock.unlock();
        
        metrics.cachedItems = lruCache_.Size();
        metrics.cacheHitRatio = lruCache_.GetHitRatio();
        
        if (enableIndexing_) {
            metrics.indexCount = indexManager_.GetIndexNames().size();
        }
        
        if (enableVersioning_) {
            std::shared_lock vlock(versionMutex_);
            for (const auto& [_, versions] : versionStore_) {
                metrics.versionCount += versions.size();
            }
        }
        
        return metrics;
    }
    
private:
    // Вспомогательные методы
    template<typename T>
    void AddVersion(const std::string& key, const T& value) {
        std::unique_lock lock(versionMutex_);
        auto& versions = versionStore_[key];
        versions.push_back(AnyData(value));
        
        // Ограничиваем количество версий
        if (versions.size() > maxVersions_) {
            versions.erase(versions.begin());
        }
    }

private:
    // Вспомогательные методы для работы с вторичными индексами 
    template<typename T>
    void AddToIndices(const std::string& key, const T& value) {
        std::shared_lock lock(extractorMutex_);
        for (const auto& [indexName, extractorInfo] : extractors_) {
            // Проверяем, что экстрактор предназначен для типа данных T
            if (extractorInfo.dataType == typeid(T)) {

                // 1. Индексы по целым числам (int)
                if (extractorInfo.fieldType == typeid(int)) {
                    auto extractor = *std::static_pointer_cast<std::function<int(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<int>(indexName)->Add(extractor(value), key);
                }
                // 2. Индексы по строкам (std::string)
                else if (extractorInfo.fieldType == typeid(std::string)) {
                    auto extractor = *std::static_pointer_cast<std::function<std::string(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<std::string>(indexName)->Add(extractor(value), key);
                }
                // 3. Индексы по числам с плавающей запятой (double)
                else if (extractorInfo.fieldType == typeid(double)) {
                    auto extractor = *std::static_pointer_cast<std::function<double(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<double>(indexName)->Add(extractor(value), key);
                }
                // 4. Индексы по float
                else if (extractorInfo.fieldType == typeid(float)) {
                    auto extractor = *std::static_pointer_cast<std::function<float(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<float>(indexName)->Add(extractor(value), key);
                }
                // 5. Индексы по длинным целым (int64_t)
                else if (extractorInfo.fieldType == typeid(int64_t)) {
                    auto extractor = *std::static_pointer_cast<std::function<int64_t(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<int64_t>(indexName)->Add(extractor(value), key);
                }
                // 6. Индексы по булевым значениям (bool)
                else if (extractorInfo.fieldType == typeid(bool)) {
                    auto extractor = *std::static_pointer_cast<std::function<bool(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<bool>(indexName)->Add(extractor(value), key);
                }
            }
        }
    }

    template<typename T>
    void UpdateIndices(const std::string& key, const T& oldValue, const T& newValue) {
        std::shared_lock lock(extractorMutex_);
        for (const auto& [indexName, extractorInfo] : extractors_) {
            // Проверяем, что экстрактор предназначен для обрабатываемого типа данных
            if (extractorInfo.dataType == typeid(T)) {

                // 1. Целочисленные индексы (int32)
                if (extractorInfo.fieldType == typeid(int)) {
                    auto extractor = *std::static_pointer_cast<std::function<int(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<int>(indexName)->Update(extractor(oldValue), extractor(newValue), key);
                }
                // 2. Строковые индексы (string)
                else if (extractorInfo.fieldType == typeid(std::string)) {
                    auto extractor = *std::static_pointer_cast<std::function<std::string(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<std::string>(indexName)->Update(extractor(oldValue), extractor(newValue), key);
                }
                // 3. Индексы по числам с двойной точностью (double)
                else if (extractorInfo.fieldType == typeid(double)) {
                    auto extractor = *std::static_pointer_cast<std::function<double(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<double>(indexName)->Update(extractor(oldValue), extractor(newValue), key);
                }
                // 4. Индексы по float (float)
                else if (extractorInfo.fieldType == typeid(float)) {
                    auto extractor = *std::static_pointer_cast<std::function<float(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<float>(indexName)->Update(extractor(oldValue), extractor(newValue), key);
                }
                // 5. Индексы по длинным целым (int64)
                else if (extractorInfo.fieldType == typeid(int64_t)) {
                    auto extractor = *std::static_pointer_cast<std::function<int64_t(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<int64_t>(indexName)->Update(extractor(oldValue), extractor(newValue), key);
                }
                // 6. Логические индексы (bool)
                else if (extractorInfo.fieldType == typeid(bool)) {
                    auto extractor = *std::static_pointer_cast<std::function<bool(const T&)>>(extractorInfo.extractorPtr);
                    indexManager_.GetIndex<bool>(indexName)->Update(extractor(oldValue), extractor(newValue), key);
                }
            }
        }
    }

    void RemoveFromIndices(const std::string& key, const AnyData& value) {
        if (value.Empty()) return;

        std::shared_lock lock(extractorMutex_);

        for (const auto& [indexName, extractorInfo] : extractors_) {
            // Проверяем, совпадает ли тип хранимых данных (напр. User) с тем, что в AnyData
            if (value.Type() == extractorInfo.dataType) {

                // Вспомогательная лямбда для уменьшения дублирования
                auto process_remove = [&](auto field_type_identity) {
                    using FieldType = decltype(field_type_identity);

                    // ВАЖНО: Мы извлекаем данные типа T (напр. User), а не FieldType!
                    // Чтобы это работало, нам нужно знать DataType во время компиляции или использовать обертку.
                    // Так как здесь DataType стерт, мы используем диспетчеризацию по известным типам данных.

                    auto invoke_extractor = [&](auto&& typed_value) {
                        using T = std::decay_t<decltype(typed_value)>;
                        auto fieldExtractor = *std::static_pointer_cast<std::function<FieldType(const T&)>>(extractorInfo.extractorPtr);
                        FieldType fieldValue = fieldExtractor(typed_value);
                        indexManager_.GetIndex<FieldType>(indexName)->Remove(fieldValue, key);
                    };

                    // Диспетчеризация по поддерживаемым DataType
                    if (value.Is<nosqldb::data::User>()) invoke_extractor(value.Get<nosqldb::data::User>());
                    else if (value.Is<nosqldb::data::Product>()) invoke_extractor(value.Get<nosqldb::data::Product>());
                    else if (value.Is<int>()) invoke_extractor(value.Get<int>());
                    else if (value.Is<std::string>()) invoke_extractor(value.Get<std::string>());
                    else if (value.Is<double>()) invoke_extractor(value.Get<double>());
                    // Добавить другие DataType, если они могут быть проиндексированы напрямую
                };

                // 1. Индексы по int (int32 в proto)
                if (extractorInfo.fieldType == typeid(int)) {
                    process_remove(int{});
                }
                // 2. Индексы по std::string
                else if (extractorInfo.fieldType == typeid(std::string)) {
                    process_remove(std::string{});
                }
                // 3. Индексы по double
                else if (extractorInfo.fieldType == typeid(double)) {
                    process_remove(double{});
                }
                // 4. Индексы по float
                else if (extractorInfo.fieldType == typeid(float)) {
                    process_remove(float{});
                }
                // 5. Индексы по int64_t
                else if (extractorInfo.fieldType == typeid(int64_t)) {
                    process_remove(int64_t{});
                }
                // 6. Индексы по bool
                else if (extractorInfo.fieldType == typeid(bool)) {
                    process_remove(bool{});
                }
            }
        }
    }
   
    template<typename DataType, typename FieldType>
    void RebuildIndex(const std::string& indexName,
                     std::function<FieldType(const DataType&)> extractor) {
        std::shared_lock lock(storeMutex_);
        auto index = indexManager_.GetIndex<FieldType>(indexName);
        if (!index) return;

        for (const auto& [key, anyData] : mainStore_) {
            DataType value;
            if (anyData.TryGet(value)) {
                auto fieldValue = extractor(value);
                index->Add(fieldValue, key);
            }
        }
    }

protected:
    void RebuildIndices() { 
        std::shared_lock lock(storeMutex_);
        std::shared_lock lockExtractor(extractorMutex_);

        for (const auto& [indexName, info] : extractors_) {
            // Диспетчеризация по FieldType (тип ключа в индексе)
            if (info.fieldType == typeid(int)) {
                DispatchDataType<int>(indexName, info);
            }
            else if (info.fieldType == typeid(std::string)) {
                DispatchDataType<std::string>(indexName, info);
            }
            else if (info.fieldType == typeid(double)) {
                DispatchDataType<double>(indexName, info);
            }
            else if (info.fieldType == typeid(int64_t)) {
                DispatchDataType<int64_t>(indexName, info);
            }
            else if (info.fieldType == typeid(float)) {
                DispatchDataType<float>(indexName, info);
            }
            else if (info.fieldType == typeid(bool)) {
                DispatchDataType<bool>(indexName, info);
            }
        }
    }

private:
    // Выполняет приведение и вызывает RebuildIndex
    template<typename DataType, typename FieldType>
    void CallRebuild(const std::string& indexName, const ExtractorInfo& info) {
        auto extractorPtr = std::static_pointer_cast<std::function<FieldType(const DataType&)>>(info.extractorPtr);
        if (extractorPtr) {
            RebuildIndex<DataType, FieldType>(indexName, *extractorPtr);
        }
    }

    // 2. Диспетчер DataType: сопоставляет сохраненный тип данных с конкретным классом
    template<typename FieldType>
    void DispatchDataType(const std::string& indexName, const ExtractorInfo& info) {
        // Поддержка Protobuf типов
        if (info.dataType == typeid(nosqldb::data::User)) {
            CallRebuild<nosqldb::data::User, FieldType>(indexName, info);
        } 
        else if (info.dataType == typeid(nosqldb::data::Product)) {
            CallRebuild<nosqldb::data::Product, FieldType>(indexName, info);
        } 
        // Поддержка примитивных типов (если они сами являются данными)
        else if (info.dataType == typeid(int))         CallRebuild<int, FieldType>(indexName, info);
        else if (info.dataType == typeid(int64_t))     CallRebuild<int64_t, FieldType>(indexName, info);
        else if (info.dataType == typeid(std::string)) CallRebuild<std::string, FieldType>(indexName, info);
        else if (info.dataType == typeid(double))      CallRebuild<double, FieldType>(indexName, info);
        else if (info.dataType == typeid(float))       CallRebuild<float, FieldType>(indexName, info);
        else if (info.dataType == typeid(bool))        CallRebuild<bool, FieldType>(indexName, info);
        else {
            throw std::runtime_error("Uncknown type for index!");
        }
    }

protected:
    // Возвращает копию всех данных для сохранения. 
    std::unordered_map<std::string, AnyData> GetAllDataSnapshot() const {
        std::shared_lock<std::shared_mutex> lock(storeMutex_);
        return mainStore_; // Возвращаем копию
    }

    // Метод для массовой загрузки (используется при старте DiskStorage)
    void LoadDataSnapshot(const std::unordered_map<std::string, AnyData>& loaded_data) {
        std::unique_lock<std::shared_mutex> lock(storeMutex_);
        mainStore_ = loaded_data;
        // Вызвать очистку кэша, так как данные изменились
        lruCache_.Clear();
        // Индексы перестроить позже в DiskStorage
    }
};

} // namespace nosqldb