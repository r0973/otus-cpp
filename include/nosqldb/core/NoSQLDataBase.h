#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include "StorageConfig.h"
#include "MemoryStorage.h"
#include "DiskStorage.h"

namespace nosqldb
{

class NoSQLDataBase {
public:
    explicit NoSQLDataBase(const StorageConfig& config = StorageConfig{})
    : config_(config)
    , storage_(std::make_unique<DiskStorage>(config))
    {}
    // Явно запрещаем копирование
    NoSQLDataBase(const NoSQLDataBase&) = delete;
    NoSQLDataBase& operator=(const NoSQLDataBase&) = delete;
    // Разрешаем перемещение
    NoSQLDataBase(NoSQLDataBase&&) = default;
    NoSQLDataBase& operator=(NoSQLDataBase&&) = default;
    ~NoSQLDataBase() = default;
    
    // Базовые операции
    template<typename T>
    bool Put(const std::string& key, const T& value) {
        return storage_->Put(key, value);
    }
    
    template<typename T>
    std::optional<T> Get(const std::string& key) {
        return storage_->Get<T>(key);
    }
    
    bool Delete(const std::string& key) {
        return storage_->Delete(key);
    }
    
    bool Exists(const std::string& key) const {
        return storage_->Exists(key);
    }
    
    size_t Size() const {
        return storage_->Size();
    }
    
    std::vector<std::string> Keys() const {
        return storage_->Keys();
    }
    
    void Clear() {
        storage_->Clear();
    }
    
    // Вторичные индексы
    template<typename DataType, typename FieldType>
    void CreateIndex(const std::string& indexName,
                    std::function<FieldType(const DataType&)> fieldExtractor) {
        storage_->CreateIndex<DataType, FieldType>(indexName, fieldExtractor);
    }
    
    template<typename FieldType>
    std::vector<std::string> QueryByIndex(const std::string& indexName,
                                         const FieldType& value) {
        return storage_->QueryByIndex<FieldType>(indexName, value);
    }
    
    template<typename FieldType>
    std::vector<std::string> QueryRange(const std::string& indexName,
                                       const FieldType& minValue,
                                       const FieldType& maxValue) {
        return storage_->QueryRange<FieldType>(indexName, minValue, maxValue);
    }
    
    // Версионность
    template<typename T>
    bool PutWithVersion(const std::string& key, const T& value) {
        return storage_->PutWithVersion(key, value);
    }
    
    template<typename T>
    std::vector<T> GetVersions(const std::string& key, size_t limit = 10) {
        return storage_->GetVersions<T>(key, limit);
    }
    
    // Метрики
    MemoryStorage::Metrics GetMetrics() const {
        return storage_->GetMetrics();
    }
    
    // Конфигурация
    const StorageConfig& GetConfig() const { return config_; }
    void UpdateConfig(const StorageConfig& newConfig) {
        // TODO: Реализовать логику применения новой конфигурации к storage_
        // Сейчас просто обновляем локальную копию конфига.
        config_ = newConfig;
    }
    
private:
    StorageConfig config_;
    std::unique_ptr<MemoryStorage> storage_;
};

} // namespace nosqldb