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
    
    StorageConfig() = default;
};

} // namespace nosqldb