#include <iostream>
#include <string>
#include <vector>
#include "NoSQLDataBase.h"

using namespace nosqldb;

struct User
{
    int id;
    std::string name;
    std::string email;
    int age;
    std::string city;
    
    bool operator==(const User& other) const
    {
        return id == other.id;
    }
};

struct Product
{
    int productId;
    std::string name;
    std::string category;
    double price;
    int stock;
};

int main()
{
    std::cout << "=== nosqldb User Storage Example ===\n" << std::endl;
    
    // Конфигурация
    StorageConfig config;
    config.lruCacheCapacity = 500;
    config.enableVersioning = true;
    config.enableIndexing = true;
    config.maxVersionsPerKey = 5;
    
    // Создание базы данных
    NoSQLDataBase db(config);
    
    // 1. Добавление пользователей
    std::cout << "1. Adding users..." << std::endl;
    
    User users[] = {
        {1, "Alice", "alice@example.com", 25, "New York"},
        {2, "Bob", "bob@company.com", 30, "London"},
        {3, "Charlie", "charlie@test.org", 22, "Paris"},
        {4, "Diana", "diana@web.dev", 28, "Berlin"},
        {5, "Eve", "eve@security.net", 35, "Tokyo"}
    };
    
    for (const auto& user : users) {
        std::string key = "user:" + std::to_string(user.id);
        db.Put(key, user);
        std::cout << "   Added: " << user.name << " (ID: " << user.id << ")" << std::endl;
    }
    
    // 2. Создание вторичных индексов
    std::cout << "\n2. Creating secondary indexes..." << std::endl;
    
    // Индекс по email
    db.CreateIndex<User, std::string>(
        "user_email_idx",
        [](const User& u) { return u.email; }
    );
    std::cout << "   Created index: user_email_idx" << std::endl;
    
    // Индекс по возрасту
    db.CreateIndex<User, int>(
        "user_age_idx",
        [](const User& u) { return u.age; }
    );
    std::cout << "   Created index: user_age_idx" << std::endl;
    
    // Индекс по городу
    db.CreateIndex<User, std::string>(
        "user_city_idx",
        [](const User& u) { return u.city; }
    );
    std::cout << "   Created index: user_city_idx" << std::endl;
    
    // 3. Запросы с использованием индексов
    std::cout << "\n3. Querying with indexes..." << std::endl;
    
    // Поиск по email
    std::cout << "   Users with email 'alice@example.com':" << std::endl;
    auto usersByEmail = db.QueryByIndex<std::string>("user_email_idx", "alice@example.com");
    for (const auto& key : usersByEmail) {
        auto user = db.Get<User>(key);
        if (user) {
            std::cout << "     - " << user->name << " (ID: " << user->id << ")" << std::endl;
        }
    }
    
    // Поиск по диапазону возрастов
    std::cout << "\n   Users aged 25-30:" << std::endl;
    auto usersByAge = db.QueryRange<int>("user_age_idx", 25, 30);
    for (const auto& key : usersByAge) {
        auto user = db.Get<User>(key);
        if (user) {
            std::cout << "     - " << user->name << " (" << user->age << " years)" << std::endl;
        }
    }
    
    // Поиск по городу
    std::cout << "\n   Users in London:" << std::endl;
    auto usersInLondon = db.QueryByIndex<std::string>("user_city_idx", "London");
    for (const auto& key : usersInLondon) {
        auto user = db.Get<User>(key);
        if (user) {
            std::cout << "     - " << user->name << std::endl;
        }
    }
    
    // 4. Обновление данных
    std::cout << "\n4. Updating data..." << std::endl;
    
    // Получаем Alice
    auto aliceKey = "user:1";
    auto aliceOpt = db.Get<User>(aliceKey);
    if (aliceOpt) {
        User updatedAlice = *aliceOpt;
        updatedAlice.age = 26;  // День рождения!
        
        // Обновляем
        db.Put(aliceKey, updatedAlice);
        std::cout << "   Updated Alice's age to " << updatedAlice.age << std::endl;
        
        // Проверяем, что индекс обновился
        auto youngUsers = db.QueryRange<int>("user_age_idx", 25, 26);
        std::cout << "   Users aged 25-26 after update: " << youngUsers.size() << std::endl;
    }
    
    // 5. Версионность
    std::cout << "\n5. Version history..." << std::endl;
    
    // Несколько обновлений
    for (int i = 1; i <= 3; i++) {
        User tempUser{6, "TempUser", "temp@test.com", 20 + i, "TestCity"};
        db.PutWithVersion("temp:user", tempUser);
    }
    
    auto versions = db.GetVersions<User>("temp:user");
    std::cout << "   Version history for temp:user:" << std::endl;
    for (size_t i = 0; i < versions.size(); i++) {
        std::cout << "     v" << i + 1 << ": " << versions[i].name 
                  << " (age: " << versions[i].age << ")" << std::endl;
    }
    
    // 6. Метрики производительности
    std::cout << "\n6. Performance metrics:" << std::endl;
    auto metrics = db.GetMetrics();
    std::cout << "   Total users: " << metrics.totalItems << std::endl;
    std::cout << "   Items in cache: " << metrics.cachedItems << std::endl;
    std::cout << "   Cache hit ratio: " << metrics.cacheHitRatio * 100 << "%" << std::endl;
    std::cout << "   Number of indexes: " << metrics.indexCount << std::endl;
    std::cout << "   Total versions stored: " << metrics.versionCount << std::endl;
    
    // 7. Удаление
    std::cout << "\n7. Cleanup..." << std::endl;
    db.Delete("temp:user");
    std::cout << "   Deleted temp:user" << std::endl;
    
    std::cout << "\n=== Example completed ===" << std::endl;
    
    return 0;
}