#include "nosqldb/core/StorageManager.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace nosqldb {

StorageManager::StorageManager(const std::string& root_path) : root_path_(root_path) {
    if (!fs::exists(root_path_)) {
        fs::create_directories(root_path_);
    }
}

std::string StorageManager::get_storage_path(const std::string& name) const {
    return (fs::path(root_path_) / name).string();
}

DiskStorage* StorageManager::OpenStorage(const std::string& name, const StorageConfig& config) {
    std::unique_lock lock(map_mutex_);

    // Если база уже открыта — возвращаем её
    if (active_storages_.count(name)) {
        return active_storages_[name].get();
    }

    // Настраиваем конфиг для конкретной папки
    StorageConfig specific_config = config;
    specific_config.dataDirectory = get_storage_path(name);
    specific_config.enablePersistence = true;

    // Создаем новый экземпляр DiskStorage (он сам вызовет Restore в конструкторе)
    auto storage = std::make_unique<DiskStorage>(specific_config);
    DiskStorage* ptr = storage.get();
    active_storages_[name] = std::move(storage);

    return ptr;
}

bool StorageManager::CloseStorage(const std::string& name) {
    std::unique_lock lock(map_mutex_);
    auto it = active_storages_.find(name);
    if (it != active_storages_.end()) {
        it->second->Save(); // Сохраняем перед закрытием
        active_storages_.erase(it);
        return true;
    }
    return false;
}

bool StorageManager::DeleteStorage(const std::string& name) {
    CloseStorage(name); // Сначала закрываем в памяти
    std::string path = get_storage_path(name);
    if (fs::exists(path)) {
        return fs::remove_all(path) > 0;
    }
    return false;
}

std::vector<std::string> StorageManager::ListAvailableStorages() const {
    std::vector<std::string> names;
    for (const auto& entry : fs::directory_iterator(root_path_)) {
        if (entry.is_directory()) {
            names.push_back(entry.path().filename().string());
        }
    }
    return names;
}

void StorageManager::SyncAll() {
    std::shared_lock lock(map_mutex_);
    for (auto& [name, storage] : active_storages_) {
        storage->Save();
    }
}

} // namespace nosqldb
