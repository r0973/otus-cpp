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
public:
    explicit MemoryStorage(const StorageConfig& config = StorageConfig())
        : mainStore_{},
          lruCache_{config.lruCacheCapacity},
          indexManager_{},
          enableIndexing_(config.enableIndexing),
          extractors_{},
          versionStore_{},
          enableVersioning_{config.enableVersioning},
          maxVersions_(config.maxVersionsPerKey){}
public:
    virtual ~MemoryStorage() = default; 

    // 2. Виртуальный метод для сохранения
    virtual void Save() {
        // Базовый класс ничего не делает
    }

    // Основные операции
    template<typename T>
    bool Put(const std::string& key, const T& value) {
        std::unique_lock lock(storeMutex_);
        
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
            std::cout << "[STORAGE] Found in cache (LRU)" << std::endl;
            return cached; // Возвращаем AnyData из кэша
        }

        // 2. Если в кэше нет, ищем в основном хранилище
        std::shared_lock lock(storeMutex_);
        std::cout << "[STORAGE] Get key from main store: " << key << std::endl;
        
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
    
    template<typename T>
    void UpdateIndices([[maybe_unused]] const std::string& key
                      ,[[maybe_unused]] const T& oldValue
                      ,[[maybe_unused]] const T& newValue) {
        std::shared_lock lock(extractorMutex_);
        // пока заглушка
    }
    
    template<typename T>
    void AddToIndices([[maybe_unused]] const std::string& key
                     ,[[maybe_unused]] const T& value) {
        std::shared_lock lock(extractorMutex_);
        // пока заглушка
    }
    
    void RemoveFromIndices([[maybe_unused]] const std::string& key
                          ,[[maybe_unused]] const AnyData& value) {
        // пока заглушка
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
        // пока заглушка
    };

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