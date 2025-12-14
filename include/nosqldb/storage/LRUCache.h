#pragma once

#include <list>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <optional>
#include <string>
#include <vector>
#include <atomic>
#include "AnyData.h"

namespace nosqldb
{

template<typename KeyType = std::string, typename ValueType = AnyData>
class LRUCache
{
public:
    explicit LRUCache(size_t capacity = 1000)
        : capacity_(capacity > 0 ? capacity : 1000) {}
    
    ~LRUCache() = default;
    
    // Основные операции
    void Put(const KeyType& key, const ValueType& value) {
        std::unique_lock lock(mutex_);
        
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end()) {
            // Обновляем существующий элемент
            it->second->second = value;
            lruList_.splice(lruList_.begin(), lruList_, it->second);
            return;
        }
        
        // Проверяем необходимость вытеснения
        if (lruList_.size() >= capacity_) {
            // Удаляем самый старый элемент
            auto last = lruList_.end();
            --last;
            cacheMap_.erase(last->first);
            lruList_.pop_back();
        }
        
        // Добавляем новый элемент
        lruList_.emplace_front(key, value);
        cacheMap_[key] = lruList_.begin();
    }
    
    std::optional<ValueType> Get(const KeyType& key) {
        std::unique_lock lock(mutex_);
        
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            ++missCount_;
            return std::nullopt;
        }
        
        // Перемещаем в начало (самый свежий)
        lruList_.splice(lruList_.begin(), lruList_, it->second);
        ++hitCount_;
        return it->second->second;
    }
    
    bool Remove(const KeyType& key) {
        std::unique_lock lock(mutex_);
        
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            return false;
        }
        
        lruList_.erase(it->second);
        cacheMap_.erase(it);
        return true;
    }
    
    bool Contains(const KeyType& key) const {
        std::shared_lock lock(mutex_);
        return cacheMap_.find(key) != cacheMap_.end();
    }
    
    void Clear() {
        std::unique_lock lock(mutex_);
        lruList_.clear();
        cacheMap_.clear();
        hitCount_ = 0;
        missCount_ = 0;
    }
    
    // Информация
    size_t Size() const {
        std::shared_lock lock(mutex_);
        return cacheMap_.size();
    }
    
    size_t Capacity() const { return capacity_; }
    
    void SetCapacity(size_t newCapacity) {
        std::unique_lock lock(mutex_);
        capacity_ = newCapacity > 0 ? newCapacity : 1;
        
        // Удаляем лишние элементы если нужно
        while (lruList_.size() > capacity_) {
            auto last = lruList_.end();
            --last;
            cacheMap_.erase(last->first);
            lruList_.pop_back();
        }
    }
    
    // Метрики
    size_t GetHitCount() const { return hitCount_; }
    size_t GetMissCount() const { return missCount_; }
    
    double GetHitRatio() const {
        size_t total = hitCount_ + missCount_;
        return total > 0 ? static_cast<double>(hitCount_) / total : 0.0;
    }
    
    // Для отладки
    std::vector<KeyType> GetKeys() const {
        std::shared_lock lock(mutex_);
        std::vector<KeyType> keys;
        keys.reserve(cacheMap_.size());
        
        // В порядке от наиболее к наименее используемым
        for (const auto& node : lruList_) {
            keys.push_back(node.first);
        }
        
        return keys;
    }
    
    // Для тестов
    std::optional<ValueType> Peek(const KeyType& key) const {
        std::shared_lock lock(mutex_);
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            return std::nullopt;
        }
        return it->second->second;
    }
    
private:
    using Node = std::pair<KeyType, ValueType>;
    using NodeList = std::list<Node>;
    using NodeMap = std::unordered_map<KeyType, typename NodeList::iterator>;
    
    NodeList lruList_;
    NodeMap cacheMap_;
    mutable std::shared_mutex mutex_;
    size_t capacity_;
    
    // Метрики
    mutable std::atomic<size_t> hitCount_{0};
    mutable std::atomic<size_t> missCount_{0};
};

} // namespace nosqldb