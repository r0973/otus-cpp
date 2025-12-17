#pragma once

#include "MemoryStorage.h"
#include "FileSegmentManager.h"
#include "StorageConfig.h"

namespace nosqldb {

class DiskStorage : public MemoryStorage {
public:
    explicit DiskStorage(const StorageConfig& config)
        : MemoryStorage(config),
          segment_manager_(config.dataDirectory, config.maxSegmentSize) {
        
        // Автоматическая загрузка при создании
        Restore();
    }

    // Сохранение текущего состояния на диск
    void Save() override {
        auto snapshot = GetAllDataSnapshot();
        segment_manager_.Save(snapshot);
    }

    // Загрузка с диска
    void Restore() {
        std::unordered_map<std::string, AnyData> loaded_data;
        segment_manager_.Load(loaded_data);
        
        if (!loaded_data.empty()) {
            LoadDataSnapshot(loaded_data);
            
            // Восстановление индексов
            // MemoryStorage должен иметь метод для пересборки индексов
            RebuildIndices(); 
        }
    }
    
private:
    FileSegmentManager segment_manager_;
};

} // namespace nosqldb