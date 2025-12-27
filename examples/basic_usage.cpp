#include <iostream>
#include <string>
#include <vector>
#include "NoSQLDataBase.h"

using namespace nosqldb;

int main() {
    std::cout << "=== NoSQLDB Basic Usage Example ===\n" << std::endl;
    
    // 1. Создание базы данных с конфигурацией
    StorageConfig config;
    config.lruCacheCapacity = 100;
    config.enableVersioning = true;
    config.enableIndexing = true;
    
    NoSQLDataBase db(config);
    
    // 2. Базовые операции
    std::cout << "1. Basic CRUD operations:" << std::endl;
    
    // Put
    db.Put("key1", 42);
    db.Put("key2", std::string("Hello, World!"));
    db.Put("key3", 3.14159);
    
    // Get
    auto intValue = db.Get<int>("key1");
    auto strValue = db.Get<std::string>("key2");
    auto doubleValue = db.Get<double>("key3");
    
    std::cout << "   key1 (int): " << (intValue ? std::to_string(*intValue) : "not found") << std::endl;
    std::cout << "   key2 (string): " << (strValue ? *strValue : "not found") << std::endl;
    std::cout << "   key3 (double): " << (doubleValue ? std::to_string(*doubleValue) : "not found") << std::endl;
    
    // Exists
    std::cout << "   key1 exists: " << (db.Exists("key1") ? "yes" : "no") << std::endl;
    std::cout << "   key4 exists: " << (db.Exists("key4") ? "yes" : "no") << std::endl;
    
    // Delete
    db.Delete("key3");
    std::cout << "   After delete key3 exists: " << (db.Exists("key3") ? "yes" : "no") << std::endl;
    
    // Size and Keys
    std::cout << "   Total items: " << db.Size() << std::endl;
    auto keys = db.Keys();
    std::cout << "   Keys: ";
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    // 3. Работа с пользовательскими типами
    std::cout << "\n2. Custom types:" << std::endl;
    
    nosqldb::data::User alice;
    alice.set_name("Alice");
    alice.set_age(25);
    alice.set_city("New York");
    
    nosqldb::data::User bob;
    bob.set_name("Bob");
    bob.set_age(30);
    bob.set_city("London");
    
    db.Put("person:1", alice);
    db.Put("person:2", bob);
    
    auto retrievedAlice = db.Get<nosqldb::data::User>("person:1");
    if (retrievedAlice && retrievedAlice->name() == "Alice") {
        std::cout << "   Person 1 retrieved correctly" << std::endl;
    }
    
    // 4. Версионность
    std::cout << "\n3. Versioning:" << std::endl;
    
    db.PutWithVersion("counter", 1);
    db.PutWithVersion("counter", 2);
    db.PutWithVersion("counter", 3);
    
    auto versions = db.GetVersions<int>("counter");
    std::cout << "   Versions of 'counter': ";
    for (int v : versions) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    
    // 5. Метрики
    std::cout << "\n4. Metrics:" << std::endl;
    auto metrics = db.GetMetrics();
    std::cout << "   Total items: " << metrics.totalItems << std::endl;
    std::cout << "   Cached items: " << metrics.cachedItems << std::endl;
    std::cout << "   Cache hit ratio: " << metrics.cacheHitRatio * 100 << "%" << std::endl;
    std::cout << "   Version count: " << metrics.versionCount << std::endl;
    
    // 6. Очистка
    std::cout << "\n5. Clear database:" << std::endl;
    db.Clear();
    std::cout << "   After clear, size: " << db.Size() << std::endl;
    
    std::cout << "\n=== Example completed successfully ===" << std::endl;
    
    return 0;
}