#include "StorageConfig.h"
#include "DiskStorage.h"
/*
int main() {
    try {
        nosqldb::StorageConfig config;
        config.dataDirectory = "./test_db";
        config.enableWAL = true;
        config.enablePersistence = true;
        
        // Тест 1: Нормальная работа
        std::cout << "Test 1: Normal operation..." << std::endl;
        {
            nosqldb::DiskStorage db(config);
            db.Put("key1", 42);
            db.Put("key2", std::string("test"));
            db.Save();  // Снимок + очистка WAL
            std::cout << "  Saved 2 keys" << std::endl;
        }
        
        // Тест 2: Имитация сбоя
        std::cout << "\nTest 2: Simulating crash after writes..." << std::endl;
        {
            nosqldb::DiskStorage db(config);
            db.Put("key3", 100);  // Записали в WAL
            db.Put("key4", 200);  // Записали в WAL
            // Сбой! Save() не вызван, WAL не очищен
            std::cout << "  Wrote 2 keys (no Save())" << std::endl;
        }
        
        // Тест 3: Восстановление после сбоя
        std::cout << "\nTest 3: Recovery after crash..." << std::endl;
        {
            nosqldb::DiskStorage db(config);
            // При создании автоматически:
            // 1. Восстановление из WAL (key3, key4)
            // 2. Восстановление из сегментов (key1, key2)
            
            auto val1 = db.Get<int>("key1");
            auto val2 = db.Get<std::string>("key2");
            auto val3 = db.Get<int>("key3");
            auto val4 = db.Get<int>("key4");
            
            std::cout << "  key1 (from segment): " << (val1 ? std::to_string(*val1) : "NOT FOUND") << std::endl;
            std::cout << "  key2 (from segment): " << (val2 ? *val2 : "NOT FOUND") << std::endl;
            std::cout << "  key3 (from WAL): " << (val3 ? std::to_string(*val3) : "NOT FOUND") << std::endl;
            std::cout << "  key4 (from WAL): " << (val4 ? std::to_string(*val4) : "NOT FOUND") << std::endl;
            
            // После восстановления делаем Save()
            db.Save();  // Очистит WAL
            std::cout << "  Recovery complete, WAL cleared" << std::endl;
        }
        
        // Тест 4: Проверка что WAL очищен
        std::cout << "\nTest 4: Check WAL is cleared..." << std::endl;
        {
            nosqldb::DiskStorage db(config);
            auto val1 = db.Get<int>("key1");
            std::cout << "  key1 (should exist): " << (val1 ? std::to_string(*val1) : "NOT FOUND") << std::endl;
        }
        
        std::cout << "\nAll tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
*/
int main() {
    try {
        nosqldb::StorageConfig config;
        config.dataDirectory = "./test_db";
        config.enableWAL = true;
        config.enablePersistence = true;
        
        std::cout << "=== Starting WAL Demo ===\n" << std::endl;
        
        // Тест 1: Нормальная работа
        std::cout << "=== Test 1: Normal operation ===" << std::endl;
        {
            std::cout << "Creating DiskStorage..." << std::endl;
            nosqldb::DiskStorage db(config);
            std::cout << "Putting key1..." << std::endl;
            db.Put("key1", 42);
            std::cout << "Putting key2..." << std::endl;
            db.Put("key2", std::string("test"));
            std::cout << "Saving..." << std::endl;
            db.Save();
            std::cout << "Test 1 complete\n" << std::endl;
        }
        
        // Тест 2: Имитация сбоя
        std::cout << "=== Test 2: Simulating crash ===" << std::endl;
        {
            std::cout << "Creating DiskStorage..." << std::endl;
            nosqldb::DiskStorage db(config);
            std::cout << "Putting key3 (WAL only)..." << std::endl;
            db.Put("key3", 100);
            std::cout << "Putting key4 (WAL only)..." << std::endl;
            db.Put("key4", 200);
            // НЕ вызываем Save() - имитируем сбой
            std::cout << "Test 2 complete (no Save called)\n" << std::endl;
        }
        
        // Проверим файл WAL
        std::cout << "=== Checking WAL file ===" << std::endl;
        {
            std::ifstream wal_file("./test_db/wal.log", std::ios::binary);
            if (wal_file) {
                wal_file.seekg(0, std::ios::end);
                std::cout << "WAL file size: " << wal_file.tellg() << " bytes" << std::endl;
            } else {
                std::cout << "WAL file not found!" << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Тест 3: Восстановление
        std::cout << "=== Test 3: Recovery ===" << std::endl;
        {
            std::cout << "Creating DiskStorage (should recover from WAL)..." << std::endl;
            nosqldb::DiskStorage db(config);
            
            // Проверим что в памяти
            std::cout << "\nChecking keys in memory store:" << std::endl;
            auto keys = db.Keys();
            std::cout << "Total keys in store: " << keys.size() << std::endl;
            for (const auto& key : keys) {
                std::cout << "  Key: " << key << std::endl;
            }
            
            auto val3 = db.Get<int>("key3");
            auto val4 = db.Get<int>("key4");
            
            std::cout << "\nRecovery results:" << std::endl;
            std::cout << "  key3 from WAL: " << (val3 ? std::to_string(*val3) : "NOT FOUND") << std::endl;
            std::cout << "  key4 from WAL: " << (val4 ? std::to_string(*val4) : "NOT FOUND") << std::endl;
            
            std::cout << "\nSaving to clear WAL..." << std::endl;
            db.Save();
            std::cout << "Test 3 complete\n" << std::endl;
        }
        
        std::cout << "=== All tests completed ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}