#pragma once

#include <unordered_map>
#include <set>
#include <shared_mutex>
#include <string>
#include <vector>
#include <any>
#include <memory>
#include <typeindex>

namespace nosqldb
{

// Базовый интерфейс индекса
class ISecondaryIndex
{
public:
    virtual ~ISecondaryIndex() = default;
    
    virtual void Add(const std::any& fieldValue, const std::string& primaryKey) = 0;
    virtual void Remove(const std::any& fieldValue, const std::string& primaryKey) = 0;
    virtual void Update(const std::any& oldValue, const std::any& newValue,
                       const std::string& primaryKey) = 0;
    
    virtual std::set<std::string> Find(const std::any& fieldValue) const = 0;
    virtual std::set<std::string> FindRange(const std::any& minValue,
                                           const std::any& maxValue) const = 0;
    
    virtual void Clear() = 0;
    virtual size_t Size() const = 0;
    virtual std::type_index GetFieldType() const = 0;
};

// Типизированная реализация
template<typename FieldType>
class TypedSecondaryIndex : public ISecondaryIndex {
public:
    void Add(const std::any& fieldValue, const std::string& primaryKey) override {
        FieldType value = std::any_cast<FieldType>(fieldValue);
        std::unique_lock lock(mutex_);
        index_[value].insert(primaryKey);
    }
    
    void Remove(const std::any& fieldValue, const std::string& primaryKey) override {
        FieldType value = std::any_cast<FieldType>(fieldValue);
        std::unique_lock lock(mutex_);
        auto it = index_.find(value);
        if (it != index_.end()) {
            it->second.erase(primaryKey);
            if (it->second.empty()) {
                index_.erase(it);
            }
        }
    }
    
    void Update(const std::any& oldValue, const std::any& newValue,
                const std::string& primaryKey) override {
        FieldType oldVal = std::any_cast<FieldType>(oldValue);
        FieldType newVal = std::any_cast<FieldType>(newValue);
        
        if (oldVal != newVal) {
            Remove(oldValue, primaryKey);
            Add(newValue, primaryKey);
        }
    }
    
    std::set<std::string> Find(const std::any& fieldValue) const override {
        FieldType value = std::any_cast<FieldType>(fieldValue);
        std::shared_lock lock(mutex_);
        auto it = index_.find(value);
        if (it != index_.end()) {
            return it->second;
        }
        return {};
    }
    
    std::set<std::string> FindRange(const std::any& minValue,
                                   const std::any& maxValue) const override {
        FieldType minVal = std::any_cast<FieldType>(minValue);
        FieldType maxVal = std::any_cast<FieldType>(maxValue);
        
        std::set<std::string> result;
        std::shared_lock lock(mutex_);
        
        for (const auto& [value, keys] : index_) {
            if (value >= minVal && value <= maxVal) {
                result.insert(keys.begin(), keys.end());
            }
        }
        return result;
    }
    
    void Clear() override {
        std::unique_lock lock(mutex_);
        index_.clear();
    }
    
    size_t Size() const override {
        std::shared_lock lock(mutex_);
        return index_.size();
    }
    
    std::type_index GetFieldType() const override {
        return typeid(FieldType);
    }
    
private:
    std::unordered_map<FieldType, std::set<std::string>> index_;
    mutable std::shared_mutex mutex_;
};

// Менеджер индексов
class SecondaryIndexManager {
public:
    SecondaryIndexManager() = default;
    
    template<typename FieldType>
    void CreateIndex(const std::string& name) {
        std::unique_lock lock(mutex_);
        if (indices_.find(name) != indices_.end()) {
            throw std::runtime_error("Index with name '" + name + "' already exists");
        }
        indices_[name] = std::make_shared<TypedSecondaryIndex<FieldType>>();
    }
    
    template<typename FieldType>
    std::shared_ptr<TypedSecondaryIndex<FieldType>> GetIndex(const std::string& name) {
        std::shared_lock lock(mutex_);
        auto it = indices_.find(name);
        if (it == indices_.end()) {
            return nullptr;
        }
        
        // Проверяем тип
        if (it->second->GetFieldType() != typeid(FieldType)) {
            throw std::runtime_error("Type mismatch for index '" + name + "'");
        }
        
        return std::static_pointer_cast<TypedSecondaryIndex<FieldType>>(it->second);
    }
    
    bool HasIndex(const std::string& name) const {
        std::shared_lock lock(mutex_);
        return indices_.find(name) != indices_.end();
    }
    
    void RemoveIndex(const std::string& name) {
        std::unique_lock lock(mutex_);
        indices_.erase(name);
    }
    
    std::vector<std::string> GetIndexNames() const {
        std::shared_lock lock(mutex_);
        std::vector<std::string> names;
        names.reserve(indices_.size());
        for (const auto& [name, _] : indices_) {
            names.push_back(name);
        }
        return names;
    }
    
    void ClearAll() {
        std::unique_lock lock(mutex_);
        indices_.clear();
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<ISecondaryIndex>> indices_;
    mutable std::shared_mutex mutex_;
};

} // namespace nosqldb