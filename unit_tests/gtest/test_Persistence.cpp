#include <gtest/gtest.h>
#include <filesystem>
#include "DiskStorage.h"
#include "StorageConfig.h"
#include "StorageManager.h"

namespace fs = std::filesystem;
using namespace nosqldb;

class PersistenceTest : public ::testing::Test {
protected:
    const std::string test_dir = "./test_db_persistence";

    void SetUp() override {
        // Очищаем тестовую директорию перед каждым тестом
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }

    void TearDown() override {
        // Очистка после тестов
        if (fs::exists(test_dir)) {
            fs::remove_all(test_dir);
        }
    }
};

TEST_F(PersistenceTest, PutSaveLoadCycle) {
    StorageConfig config;
    config.dataDirectory = test_dir;
    config.enablePersistence = true;
    config.maxSegmentSize = 10;

    // 1. Создаем первый объект и кладем данные
    {
        DiskStorage storage(config);
        storage.Put("user:1", std::string("Alice"));
        storage.Put("user:2", 42);
        
        // Сохраняем на диск
        storage.Save();
        
        // Проверяем физическое наличие файла
        EXPECT_TRUE(fs::exists(test_dir + "/segment_0.db"));
    }

    // 2. Создаем второй объект (новый экземпляр),
	//    он должен вызвать Restore() в конструкторе
    {
        DiskStorage storage(config);
        
        // Проверяем восстановление данных
        auto val1 = storage.Get<std::string>("user:1");
        auto val2 = storage.Get<int>("user:2");

        ASSERT_TRUE(val1.has_value());
        ASSERT_TRUE(val2.has_value());
        EXPECT_EQ(*val1, "Alice");
        EXPECT_EQ(*val2, 42);
    }
}

TEST_F(PersistenceTest, DISABLED_IndexRestorationAfterLoad) {
    StorageConfig config;
    config.dataDirectory = test_dir;
    config.enablePersistence = true;

    // 1. Заполняем данными и сохраняем
    {
        DiskStorage storage(config);
        storage.Put("key1", 10);
        storage.Put("key2", 20);
        storage.Put("key3", 30);
        storage.Save();
    }

    // 2. В новом объекте сначала создаем индекс, потом загружаем данные
    {
        DiskStorage storage(config);
        
        // Регистрируем индекс (как он был бы зарегистрирован в приложении)
        storage.CreateIndex<int, int>("val_idx", [](const int& v) { return v; });
        
        // Restore происходит в конструкторе, но нам нужно убедиться, 
        // что индекс перестроился. Если Restore в конструкторе не вызвал 
        // RebuildAllIndices, данные в индексе будут пусты.
        
        auto results = storage.QueryRange<int>("val_idx", 15, 35);
        
        EXPECT_EQ(results.size(), 2); // Должны найти 20 и 30
    }
}

TEST_F(PersistenceTest, SegmentationCheck) {
    StorageConfig config;
    config.dataDirectory = test_dir;
    config.enablePersistence = true;
    config.maxSegmentSize = 2; // Каждые 2 записи — новый файл

    {
        DiskStorage storage(config);
        storage.Put("k1", 1);
        storage.Put("k2", 2);
        storage.Put("k3", 3);
        storage.Put("k4", 4);
        storage.Put("k5", 5);
        
        storage.Save();
        
        // Должно создаться 3 сегмента (2+2+1)
        int segment_count = 0;
        for (auto const& dir_entry : fs::directory_iterator(test_dir)) {
            if (dir_entry.path().extension() == ".db") segment_count++;
        }
        EXPECT_EQ(segment_count, 3);
    }
}

TEST(StorageManagerTest, ManageMultipleDBs) {
    nosqldb::StorageManager manager("./multi_db_root");
    
    // Открываем две разные базы
    auto* db_users = manager.OpenStorage("users", nosqldb::StorageConfig{});
    auto* db_logs = manager.OpenStorage("logs", nosqldb::StorageConfig{});

    db_users->Put("admin", std::string("root"));
    db_logs->Put("event_1", std::string("login_success"));

    manager.SyncAll();

    // Проверяем физическое разделение
    EXPECT_TRUE(fs::exists("./multi_db_root/users/segment_0.db"));
    EXPECT_TRUE(fs::exists("./multi_db_root/logs/segment_0.db"));

    EXPECT_EQ(manager.ListAvailableStorages().size(), 2);
    
    fs::remove_all("./multi_db_root");
}