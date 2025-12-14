#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "NoSQLDataBase.h"

using namespace nosqldb;

class ConcurrentTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.lruCacheCapacity = 1000;
        config.enableIndexing = false; //TODO: временно отключили индексы.
        db = std::make_unique<NoSQLDataBase>(config);
    }
    
    void TearDown() override {
        db.reset();
    }
    
    StorageConfig config;
    std::unique_ptr<NoSQLDataBase> db;
};

TEST_F(ConcurrentTest, AnyDataThreadSafety) {
    std::cout << "=== Testing AnyData thread safety ===" << std::endl;
    
    AnyData data(42);
    constexpr int NUM_THREADS = 4;
    constexpr int READS_PER_THREAD = 1000;
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int t = 0; t < NUM_THREADS; t++) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < READS_PER_THREAD; i++) {
                int value;
                if (data.TryGet(value) && value == 42) {
                    successCount++;
                }
            }
        });
    }
    
    for (auto& t : threads) t.join();
    
    std::cout << "AnyData concurrent reads: " << successCount 
              << "/" << NUM_THREADS * READS_PER_THREAD << std::endl;
    EXPECT_EQ(successCount, NUM_THREADS * READS_PER_THREAD);
}

TEST_F(ConcurrentTest, LRUCacheThreadSafety) {
    LRUCache<std::string, int> cache(100);
    
    constexpr int NUM_THREADS = 4;
    constexpr int OPS_PER_THREAD = 100;
    
    std::vector<std::thread> threads;
    std::atomic<int> puts{0};
    std::atomic<int> gets{0};
    
    for (int t = 0; t < NUM_THREADS; t++) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < OPS_PER_THREAD; i++) {
                std::string key = "t" + std::to_string(t) + "_k" + std::to_string(i);
                cache.Put(key, i);
                puts++;
                
                // Иногда читаем
                if (i % 10 == 0) {
                    auto val = cache.Get(key);
                    if (val.has_value()) {
                        gets++;
                    }
                }
            }
        });
    }
    
    for (auto& t : threads)
        t.join();
    
    std::cout << "LRUCache: " << puts << " puts, " << gets << " gets" << std::endl;
    EXPECT_EQ(puts, NUM_THREADS * OPS_PER_THREAD);
}

TEST_F(ConcurrentTest, SimplePutGetNoLRU) {
    std::cout << "=== Simple Put/Get without LRU ===" << std::endl;
    
    StorageConfig config;
    config.lruCacheCapacity = 0;  // NO LRU cache
    config.enableIndexing = false;
    NoSQLDataBase db(config);
    
    // Просто один поток
    EXPECT_TRUE(db.Put("test1", 42));
    auto val = db.Get<int>("test1");
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);
    
    std::cout << "Single thread test PASSED" << std::endl;
    
    // Два потока с разными ключами
    std::thread t1([&]() {
        for (int i = 0; i < 10; i++) {
            db.Put("t1_" + std::to_string(i), i);
            auto v = db.Get<int>("t1_" + std::to_string(i));
            EXPECT_TRUE(v.has_value());
        }
    });
    
    std::thread t2([&]() {
        for (int i = 0; i < 10; i++) {
            db.Put("t2_" + std::to_string(i), i*2);
            auto v = db.Get<int>("t2_" + std::to_string(i));
            EXPECT_TRUE(v.has_value());
        }
    });
    
    t1.join();
    t2.join();
    
    std::cout << "Two threads test PASSED" << std::endl;
}

TEST_F(ConcurrentTest, ConcurrentPuts) {
    constexpr int NUM_THREADS = 8;
    constexpr int OPERATIONS_PER_THREAD = 1000;
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int t = 0; t < NUM_THREADS; t++) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
                std::string key = "thread" + std::to_string(t) + "_key" + std::to_string(i);
                if (db->Put(key, i)) {
                    successCount++;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount, NUM_THREADS * OPERATIONS_PER_THREAD);
    EXPECT_EQ(db->Size(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

TEST_F(ConcurrentTest, ConcurrentPutsAndGets) {
    // Сначала заполняем
    for (int i = 0; i < 100; i++) {
        db->Put("key" + std::to_string(i), i);
    }
    
    constexpr int NUM_THREADS = 4;
    constexpr int OPERATIONS_PER_THREAD = 500;
    
    std::vector<std::thread> threads;
    std::atomic<int> getSuccessCount{0};
    std::atomic<int> putSuccessCount{0};
    
    for (int t = 0; t < NUM_THREADS; t++) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
                if (i % 3 == 0) {
                    // Put операция
                    std::string key = "t" + std::to_string(t) + "_k" + std::to_string(i);
                    if (db->Put(key, i)) {
                        putSuccessCount++;
                    }
                } else {
                    // Get операция
                    int keyNum = i % 100;
                    auto val = db->Get<int>("key" + std::to_string(keyNum));
                    if (val.has_value() && *val == keyNum) {
                        getSuccessCount++;
                    }
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_GT(getSuccessCount, 0);
    EXPECT_GT(putSuccessCount, 0);
    
    // Проверяем, что данные не испорчены
    for (int i = 0; i < 100; i++) {
        auto val = db->Get<int>("key" + std::to_string(i));
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(*val, i);
    }
}

// Тест производительности (опционально, можно запускать отдельно)
TEST_F(ConcurrentTest, PerformanceTest) {
    constexpr int NUM_OPERATIONS = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        db->Put("perf_key" + std::to_string(i), i);
    }
    
    auto mid = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        auto val = db->Get<int>("perf_key" + std::to_string(i));
        EXPECT_TRUE(val.has_value());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto put_time = std::chrono::duration_cast<std::chrono::milliseconds>(mid - start);
    auto get_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - mid);
    
    std::cout << "Performance results:" << std::endl;
    std::cout << "  " << NUM_OPERATIONS << " puts took: " << put_time.count() << "ms" << std::endl;
    std::cout << "  " << NUM_OPERATIONS << " gets took: " << get_time.count() << "ms" << std::endl;
    std::cout << "  Average put: " << (put_time.count() * 1000.0 / NUM_OPERATIONS) << "μs" << std::endl;
    std::cout << "  Average get: " << (get_time.count() * 1000.0 / NUM_OPERATIONS) << "μs" << std::endl;
}

class SimpleStorage {
private:
    std::unordered_map<std::string, int> data_;
    mutable std::shared_mutex mutex_;
    
public:
    bool Put(const std::string& key, int value) {
        std::unique_lock lock(mutex_);
        data_[key] = value;
        return true;
    }
    
    std::optional<int> Get(const std::string& key) {
        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        if (it != data_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

TEST(SimpleStorageTest, BasicTest) {
    SimpleStorage storage;
    EXPECT_TRUE(storage.Put("key1", 42));
    auto val = storage.Get("key1");
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);
    std::cout << "SimpleStorage test PASSED" << std::endl;
}

TEST(AnyDataDebugTest, ConstructionAndAccess) {
    std::cout << "=== Testing AnyData ===" << std::endl;
    
    // Тест 1: int
    std::cout << "1. Testing int..." << std::endl;
    AnyData intData(42);
    EXPECT_TRUE(intData.Is<int>());
    EXPECT_EQ(intData.Get<int>(), 42);
    
    // Тест 2: string
    std::cout << "2. Testing string..." << std::endl;
    AnyData strData(std::string("hello"));
    EXPECT_TRUE(strData.Is<std::string>());
    EXPECT_EQ(strData.Get<std::string>(), "hello");
    
    // Тест 3: custom type
    std::cout << "3. Testing custom type..." << std::endl;
    struct Point { int x, y; };
    Point p{10, 20};
    AnyData pointData(p);
    EXPECT_TRUE(pointData.Is<Point>());
    EXPECT_EQ(pointData.Get<Point>().x, 10);
    
    std::cout << "=== AnyData tests PASSED ===" << std::endl;
}

class MemoryStorageNoLRU {
private:
    std::unordered_map<std::string, AnyData> data_;
    mutable std::shared_mutex mutex_;
    
public:
    template<typename T>
    bool Put(const std::string& key, const T& value) {
        std::unique_lock lock(mutex_);
        data_[key] = AnyData(value);
        return true;
    }
    
    template<typename T>
    std::optional<T> Get(const std::string& key) {
        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }
        
        T value;
        if (it->second.TryGet(value)) {
            return value;
        }
        return std::nullopt;
    }
};

TEST(MemoryStorageNoLRUTest, BasicTest) {
    std::cout << "=== Testing MemoryStorage without LRU ===" << std::endl;
    
    MemoryStorageNoLRU storage;
    
    // Test int
    EXPECT_TRUE(storage.Put("int_key", 42));
    auto intVal = storage.Get<int>("int_key");
    EXPECT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 42);
    
    // Test string
    EXPECT_TRUE(storage.Put("str_key", std::string("test")));
    auto strVal = storage.Get<std::string>("str_key");
    EXPECT_TRUE(strVal.has_value());
    EXPECT_EQ(*strVal, "test");
    
    std::cout << "=== MemoryStorageNoLRU test PASSED ===" << std::endl;
}

TEST(LRUCacheWithAnyDataTest, BasicTest) {
    std::cout << "=== Testing LRUCache with AnyData ===" << std::endl;
    
    LRUCache<std::string, AnyData> cache(10);
    
    // Put
    cache.Put("key1", AnyData(42));
    cache.Put("key2", AnyData(std::string("hello")));
    
    // Get
    auto val1 = cache.Get("key1");
    EXPECT_TRUE(val1.has_value());
    int intVal;
    EXPECT_TRUE(val1->TryGet(intVal));
    EXPECT_EQ(intVal, 42);
    
    auto val2 = cache.Get("key2");
    EXPECT_TRUE(val2.has_value());
    std::string strVal;
    EXPECT_TRUE(val2->TryGet(strVal));
    EXPECT_EQ(strVal, "hello");
    
    std::cout << "=== LRUCache with AnyData test PASSED ===" << std::endl;
}