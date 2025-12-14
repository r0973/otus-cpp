#ifndef NOSQLSTORAGE_H
#define NOSQLSTORAGE_H

#include <unordered_map>
#include <vector>
#include <mutex>
#include <ctime>
#include <stdexcept>
#include <optional>

// Базовое NoSQL-хранилище с поддержкой версионности и потокобезопасности
template<typename K, typename V>
class NoSQLStorage {
private:
    // Хранилище: ключ -> вектор пар (значение, метка времени)
    std::unordered_map<K, std::vector<std::pair<V, std::time_t>>> data_;
    // Мьютекс для потокобезопасности
    mutable std::mutex mutex_;

public:
    // Сохранить значение по ключу (добавляет новую версию)
    void put(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_[key].emplace_back(value, std::time(nullptr));
    }

    // Получить последнюю версию значения по ключу
    V get(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.find(key) == data_.end() || data_[key].empty()) {
            throw std::runtime_error("Key not found");
        }
        return data_[key].back().first;
    }

    // Получить конкретную версию значения по ключу и индексу версии
    V getVersion(const K& key, size_t version) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.find(key) == data_.end() || version >= data_[key].size()) {
            throw std::runtime_error("Key or version not found");
        }
        return data_[key][version].first;
    }

    // Получить все версии значения по ключу
    std::vector<std::pair<V, std::time_t>> getAllVersions(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.find(key) == data_.end()) {
            throw std::runtime_error("Key not found");
        }
        return data_[key];
    }

    // Проверить существование ключа
    bool contains(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.find(key) != data_.end();
    }

    // Удалить ключ и все его версии
    void remove(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.erase(key);
    }

    // Очистить всё хранилище
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.clear();
    }
};

#endif // NOSQLSTORAGE_H