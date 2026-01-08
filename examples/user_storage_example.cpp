#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "NoSQLDataBase.h"
#include "protos/user.pb.h"
#include "protos/product.pb.h"

using namespace nosqldb;

int main()
{
    std::cout << "=== nosqldb Protobuf Storage Example (2026) ===\n" << std::endl;
    
    // Конфигурация
    StorageConfig config;
    config.lruCacheCapacity = 500;
    config.enableVersioning = true;
    config.enableIndexing = true;
    config.maxVersionsPerKey = 5;
    config.dataDirectory = "./example_db"; 
    
    // Создание базы данных
    NoSQLDataBase db(config);
    
    // 1. Добавление пользователей
    std::cout << "1. Adding users (Protobuf)..." << std::endl;
    
    // Вспомогательная функция для создания объектов Protobuf
    auto create_user = [](int id, std::string name, std::string email, int age, std::string city) {
        data::User u;
        u.set_id(id);
        u.set_name(name);
        u.set_email(email);
        u.set_age(age);
        u.set_city(city);
        return u;
    };

    std::vector<data::User> users = {
        create_user(1, "Alice", "alice@example.com", 25, "New York"),
        create_user(2, "Bob", "bob@company.com", 30, "London"),
        create_user(3, "Charlie", "charlie@test.org", 22, "Paris"),
        create_user(4, "Diana", "diana@web.dev", 28, "Berlin"),
        create_user(5, "Eve", "eve@security.net", 35, "Tokyo")
    };
    
    for (const auto& user : users) {
        std::string key = "user:" + std::to_string(user.id());
        db.Put(key, user);
        std::cout << "   Added: " << user.name() << " (ID: " << user.id() << ")" << std::endl;
    }
    
    // 2. Создание вторичных индексов
    std::cout << "\n2. Creating secondary indexes..." << std::endl;
    
    // Индекс по email (string)
    db.CreateIndex<data::User, std::string>(
        "user_email_idx",
        [](const data::User& u) { return u.email(); }
    );
    
    // Индекс по возрасту (int)
    db.CreateIndex<data::User, int>(
        "user_age_idx",
        [](const data::User& u) { return u.age(); }
    );
    
    // Индекс по городу (string)
    db.CreateIndex<data::User, std::string>(
        "user_city_idx",
        [](const data::User& u) { return u.city(); }
    );
    
    // 3. Запросы с использованием индексов
    std::cout << "\n3. Querying with indexes..." << std::endl;
    
    // Поиск по email
    std::cout << "   Users with email 'alice@example.com':" << std::endl;
    auto usersByEmail = db.QueryByIndex<std::string>("user_email_idx", "alice@example.com");
    for (const auto& key : usersByEmail) {
        auto userOpt = db.Get<data::User>(key);
        if (userOpt) {
            std::cout << "     - " << userOpt->name() << " (ID: " << userOpt->id() << ")" << std::endl;
        }
    }
    
    // Поиск по диапазону возрастов
    std::cout << "\n   Users aged 25-30:" << std::endl;
    auto usersByAge = db.QueryRange<int>("user_age_idx", 25, 30);
    for (const auto& key : usersByAge) {
        auto userOpt = db.Get<data::User>(key);
        if (userOpt) {
            std::cout << "     - " << userOpt->name() << " (" << userOpt->age() << " years)" << std::endl;
        }
    }
    
    // 4. Обновление данных
    std::cout << "\n4. Updating data..." << std::endl;
    
    auto aliceKey = "user:1";
    auto aliceOpt = db.Get<data::User>(aliceKey);
    if (aliceOpt) {
        data::User updatedAlice = *aliceOpt;
        updatedAlice.set_age(26); // Protobuf сеттер
        
        db.Put(aliceKey, updatedAlice);
        std::cout << "   Updated Alice's age to " << updatedAlice.age() << std::endl;
        
        auto youngUsers = db.QueryRange<int>("user_age_idx", 25, 26);
        std::cout << "   Keys in age range 25-26 after update: " << youngUsers.size() << std::endl;
    }
    
    // 5. Версионность
    std::cout << "\n5. Version history..." << std::endl;
    
    const std::string tempKey = "temp:user";
    for (int i = 1; i <= 3; i++) {
        data::User tempUser = create_user(6, "TempUser", "temp@test.com", 20 + i, "TestCity");
        db.PutWithVersion(tempKey, tempUser);
    }
    
    auto versions = db.GetVersions<data::User>(tempKey);
    std::cout << "   Version history for " << tempKey << ":" << std::endl;
    for (size_t i = 0; i < versions.size(); i++) {
        std::cout << "     v" << i + 1 << ": " << versions[i].name() 
                  << " (age: " << versions[i].age() << ")" << std::endl;
    }
    
    // 6. Пример с Product
    std::cout << "\n6. Working with Products..." << std::endl;
    data::Product prod;
    prod.set_product_id(101);
    prod.set_name("Quantum Computer");
    prod.set_price(999999.99);
    prod.set_category("Hardware");
    
    db.Put("prod:101", prod);
    auto retrievedProd = db.Get<data::Product>("prod:101");
    if (retrievedProd) {
        std::cout << "   Retrieved Product: " << retrievedProd->name() 
                  << " ($" << retrievedProd->price() << ")" << std::endl;
    }

    // 7. Метрики
    std::cout << "\n7. Performance metrics:" << std::endl;
    auto metrics = db.GetMetrics();
    std::cout << "   Total items: " << metrics.totalItems << std::endl;
    std::cout << "   Number of indexes: " << metrics.indexCount << std::endl;
    
    std::cout << "\n=== Example completed ===" << std::endl;
    
    return 0;
}
