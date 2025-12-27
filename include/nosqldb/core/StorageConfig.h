#pragma once

#include <cstddef>
#include <string>

namespace nosqldb
{

struct StorageConfig {
    // Размер LRU кэша
    size_t lruCacheCapacity = 1000;
    
    // Настройки потоков
    size_t threadPoolSize = 4;
    
    // Версионность
    bool enableVersioning = false;
    size_t maxVersionsPerKey = 10;
    
    // Индексы
    bool enableIndexing = false;  // пока отключено
    
    // Логирование
    bool enableLogging = true;
    std::string logLevel = "INFO"; // DEBUG, INFO, WARN, ERROR
    
    // Производительность
    bool asyncOperations = false;
    
    // Метрики
    bool collectMetrics = true;
    
    // Персистентность данных
    bool enablePersistence = false;

    // Путь
    std::string dataDirectory = "./db_data"; 
    
    // Размер сегмента
    size_t maxSegmentSize = 5000; // Количество записей в одном файле

    // WAL для отказоустойчивости
    bool enableWAL = true;  // По умолчанию включено

    // WAL настройки
    size_t walFlushInterval = 1000; // ms
    size_t maxWALSize = 100 * 1024 * 1024; // 100MB

    StorageConfig() = default;
};

} // namespace nosqldb