#pragma once

#include "MemoryStorage.h"
#include "FileSegmentManager.h"
#include "StorageConfig.h"

namespace nosqldb {

class DiskStorage : public MemoryStorage {
public:
    explicit DiskStorage(const StorageConfig& config)
        : MemoryStorage(config),
          segment_manager_(config.dataDirectory, config.maxSegmentSize)
    {
        // 1. Загружаем стабильный снимок с диска (Snapshot)
        Restore();
        // 2. Если WAL включен в конфиге - накатываем логи и инициализируем менеджер
        if (config.enableWAL) {
            // Метод RecoverFromWAL внутри должен сделать: 
            // а) Прочитать файл б) Создать объект wal_manager_ для будущих записей
            RecoverFromWAL(config.dataDirectory);
        }
        // 3. Только теперь перестраиваем индексы (один раз для всех данных)
        RebuildIndices();
    }

    // Сохранение текущего состояния на диск
    void Save() override {
        auto snapshot = GetAllDataSnapshot();
        segment_manager_.Save(snapshot);

        // Очищаем WAL после успешного сохранения
        if (wal_manager_) {
            wal_manager_->Clear();
        }
    }

    // Загрузка с диска
    void Restore() {
        std::unordered_map<std::string, AnyData> loaded_data;
        segment_manager_.Load(loaded_data);
        
        if (!loaded_data.empty()) {
            LoadDataSnapshot(loaded_data);
        }
    }
    
private:
    FileSegmentManager segment_manager_;
};

} // namespace nosqldb