#include <gtest/gtest.h>
#include <filesystem>
#include "NoSQLDataBase.h"
#include "protos/user.pb.h"
#include "protos/product.pb.h"

using namespace nosqldb;
namespace fs = std::filesystem;

class DBIndexingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Путь к тестовой директории
        test_path_ = "./db_test_dir";
        
        // Гарантируем чистоту перед тестом
        if (fs::exists(test_path_)) {
            fs::remove_all(test_path_);
        }
        fs::create_directories(test_path_);

        // Настройка конфигурации
        config_.dataDirectory = test_path_;
        config_.enableIndexing = true;
        
        // Инициализация БД
        db_ = std::make_unique<NoSQLDataBase>(config_);
    }

    void TearDown() override {
        // Освобождаем ресурсы БД перед удалением файлов
        db_.reset(); 
        
        // Удаляем временные файлы
        if (fs::exists(test_path_)) {
            fs::remove_all(test_path_);
        }
    }

    std::string test_path_;
    StorageConfig config_;
    std::unique_ptr<NoSQLDataBase> db_;
};

// Тест создания индекса и добавления данных
TEST_F(DBIndexingTest, CreateIndexAndAddData) {
    
    // Подготовка данных Protobuf
    data::User u1;
    u1.set_id(1); u1.set_name("Alice"); u1.set_age(25);
    
    data::User u2;
    u2.set_id(2); u2.set_name("Bob"); u2.set_age(30);

    // Сохранение
    db_->Put("user1", u1);
    db_->Put("user2", u2);

    // Создаём индекс по полю age у User
    db_->CreateIndex<data::User, int>(
        "user_age_index",
        [](const data::User& user) { return user.age(); }
    );

    // Проверка запросов по индексу
    auto res25 = db_->QueryByIndex<int>("user_age_index", 25);
    ASSERT_EQ(res25.size(), 1);
    EXPECT_EQ(res25[0], "user1");

    auto res30 = db_->QueryByIndex<int>("user_age_index", 30);
    ASSERT_EQ(res30.size(), 1);
    EXPECT_EQ(res30[0], "user2");
}

// Тест обновления данных (проверка корректности переиндексации)
TEST_F(DBIndexingTest, UpdateIndexOnDataChange) {
    db_->CreateIndex<data::User, int>(
        "user_age_index",
        [](const data::User& user) { return user.age(); }
    );

    data::User u1;
    u1.set_id(1); u1.set_name("Alice"); u1.set_age(25);
    db_->Put("user1", u1);

    // Обновляем возраст (25 -> 26)
    u1.set_age(26);
    db_->Put("user1", u1);

    // Старый индекс должен быть пуст
    EXPECT_TRUE(db_->QueryByIndex<int>("user_age_index", 25).empty());
    
    // Новый индекс должен содержать ключ
    auto res26 = db_->QueryByIndex<int>("user_age_index", 26);
    ASSERT_EQ(res26.size(), 1);
    EXPECT_EQ(res26[0], "user1");
}

// Тест удаления данных из индекса
TEST_F(DBIndexingTest, RemoveDataFromIndex) {
    db_->CreateIndex<data::User, std::string>(
        "user_name_index",
        [](const data::User& user) { return user.name(); }
    );

    data::User u1;
    u1.set_id(1); u1.set_name("Alice");
    db_->Put("user1", u1);

    // Удаляем из базы
    db_->Delete("user1");

    // Индекс должен стать пустым
    auto res = db_->QueryByIndex<std::string>("user_name_index", "Alice");
    EXPECT_TRUE(res.empty());
}

// Тест на поиск по диапазону в индексах
TEST_F(DBIndexingTest, QueryRangeTest) {
    // 1. Создаём индекс по полю age (тип int)
    db_->CreateIndex<data::User, int>(
        "user_age_index",
        [](const data::User& user) { return user.age(); }
    );

    // 2. Вспомогательная функция для быстрого создания пользователей
    auto create_user = [](int id, std::string name, int age) {
        data::User u;
        u.set_id(id);
        u.set_name(name);
        u.set_age(age);
        return u;
    };

    // 3. Добавляем пользователей с разными возрастами
    db_->Put("user1", create_user(1, "Alice", 20));
    db_->Put("user2", create_user(2, "Bob", 25));
    db_->Put("user3", create_user(3, "Charlie", 30));
    db_->Put("user4", create_user(4, "Diana", 35));

    // 4. Ищем пользователей в возрастном диапазоне [25, 35] (включая границы)
    auto usersInRange = db_->QueryRange<int>("user_age_index", 25, 35);

    // 5. Проверки
    // Ожидаем 3 пользователей: Bob (25), Charlie (30), Diana (35)
    ASSERT_EQ(usersInRange.size(), 3) << "Should find exactly 3 users in range [25, 35]";

    // Проверяем наличие конкретных ключей (порядок может зависеть от реализации индекса)
    std::sort(usersInRange.begin(), usersInRange.end());
    
    EXPECT_EQ(usersInRange[0], "user2"); // Bob (25)
    EXPECT_EQ(usersInRange[1], "user3"); // Charlie (30)
    EXPECT_EQ(usersInRange[2], "user4"); // Diana (35)

    // 6. Дополнительная проверка: диапазон, в который никто не попадает
    auto emptyRange = db_->QueryRange<int>("user_age_index", 40, 50);
    EXPECT_TRUE(emptyRange.empty());
}

// Тест проверки перестроения индексов
TEST_F(DBIndexingTest, RestoreIndicesAfterRestart) {
    // --- ЭТАП 1: Создание данных ---
    {
        // База инициализирована в SetUp() (db_)
        db_->CreateIndex<data::User, int>(
            "user_age_index",
            [](const data::User& user) { return user.age(); }
        );

        data::User u1; u1.set_id(1); u1.set_name("Alice"); u1.set_age(25);
        data::User u2; u2.set_id(2); u2.set_name("Bob"); u2.set_age(30);

        db_->Put("user1", u1);
        db_->Put("user2", u2);
    
        // Проверяем запросы по индексу
        auto usersWithAge25 = db_->QueryByIndex<int>("user_age_index", 25);
        ASSERT_EQ(usersWithAge25.size(), 1) << "Index recovery failed for Alice";
        EXPECT_EQ(usersWithAge25[0], "user1");

        auto usersWithAge30 = db_->QueryByIndex<int>("user_age_index", 30);
        ASSERT_EQ(usersWithAge30.size(), 1) << "Index recovery failed for Bob";
        EXPECT_EQ(usersWithAge30[0], "user2");    

        // Здесь db_ выходит из области видимости или принудительно сбрасывается, 
        // имитируя закрытие приложения.
        db_.reset(); 
    }

    // --- ЭТАП 2: Перезапуск ---
    // Создаем новый экземпляр базы, указывая ту же директорию
    db_ = std::make_unique<NoSQLDataBase>(config_);

    // Функции-экстракторы пока не сохраняются в файлах .db,
    // поэтому нужно их зарегистрировать повторно. 
    // После регистрации CreateIndex автоматически вызовет RebuildIndex по данным с диска.
    db_->CreateIndex<data::User, int>(
        "user_age_index",
        [](const data::User& user) { return user.age(); }
    );

    // --- ЭТАП 3: Проверка восстановления ---
    // Проверяем поиск по индексу, который был перестроен из файлов сегментов
    auto usersWithAge25 = db_->QueryByIndex<int>("user_age_index", 25);
    ASSERT_EQ(usersWithAge25.size(), 1) << "Index recovery failed for Alice";
    EXPECT_EQ(usersWithAge25[0], "user1");

    auto usersWithAge30 = db_->QueryByIndex<int>("user_age_index", 30);
    ASSERT_EQ(usersWithAge30.size(), 1) << "Index recovery failed for Bob";
    EXPECT_EQ(usersWithAge30[0], "user2");

    // Проверяем, что сами данные тоже на месте
    // AnyData data = db_->Get("user1");
    // ASSERT_FALSE(data.Empty());
    // EXPECT_EQ(data.Get<data::User>().name(), "Alice");

    auto userOpt = db_->Get<data::User>("user1");
    ASSERT_TRUE(userOpt.has_value()) << "User 'user1' should exist in DB";
    EXPECT_EQ(userOpt->name(), "Alice");
    EXPECT_EQ(userOpt->age(), 25);
}