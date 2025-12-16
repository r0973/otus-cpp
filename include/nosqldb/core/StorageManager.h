#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <shared_mutex>
#include <vector>
#include "DiskStorage.h"
#include "StorageConfig.h"

namespace nosqldb {

class StorageManager {
public:
    // root_path — общая папка для всех БД (например, "./databases")
    explicit StorageManager(const std::string& root_path);

    // Открывает существующую или создает новую БД с заданным именем
    // Путь к файлам будет: root_path / name / segment_X.db
    DiskStorage* OpenStorage(const std::string& name, const StorageConfig& config);

    // Закрывает хранилище и удаляет его из памяти (данные на диске остаются)
    bool CloseStorage(const std::string& name);

    // Удаляет хранилище полностью (включая файлы на диске)
    bool DeleteStorage(const std::string& name);

    // Список имен всех доступных (созданных) БД в папке root_path
    std::vector<std::string> ListAvailableStorages() const;

    // Принудительное сохранение всех открытых БД на диск
    void SyncAll();

private:
    std::string root_path_;
    mutable std::shared_mutex map_mutex_;
    std::unordered_map<std::string, std::unique_ptr<DiskStorage>> active_storages_;

    std::string get_storage_path(const std::string& name) const;
};

} // namespace nosqldb
