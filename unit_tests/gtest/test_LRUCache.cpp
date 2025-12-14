#include <gtest/gtest.h>
#include <thread>
#include "LRUCache.h"

using namespace nosqldb;

class LRUCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        cache = std::make_unique<LRUCache<std::string, int>>(3);
    }
    
    void TearDown() override {
        cache.reset();
    }
    
    std::unique_ptr<LRUCache<std::string, int>> cache;
};

TEST_F(LRUCacheTest, PutAndGet) {
    cache->Put("key1", 1);
    cache->Put("key2", 2);
    cache->Put("key3", 3);
    
    EXPECT_EQ(cache->Get("key1").value(), 1);
    EXPECT_EQ(cache->Get("key2").value(), 2);
    EXPECT_EQ(cache->Get("key3").value(), 3);
    EXPECT_EQ(cache->Size(), 3);
}

TEST_F(LRUCacheTest, LRUEviction) {
    cache->Put("key1", 1);
    cache->Put("key2", 2);
    cache->Put("key3", 3);
    
    // key1 становится самым свежим
    cache->Get("key1");
    
    // Добавляем key4, key2 должен быть вытеснен
    cache->Put("key4", 4);
    
    EXPECT_EQ(cache->Size(), 3);
    EXPECT_FALSE(cache->Get("key2").has_value());
    EXPECT_TRUE(cache->Get("key1").has_value());
    EXPECT_TRUE(cache->Get("key3").has_value());
    EXPECT_TRUE(cache->Get("key4").has_value());
}

TEST_F(LRUCacheTest, RemoveOperation) {
    cache->Put("key1", 1);
    cache->Put("key2", 2);
    
    EXPECT_TRUE(cache->Remove("key1"));
    EXPECT_FALSE(cache->Remove("key3")); // Несуществующий
    
    EXPECT_EQ(cache->Size(), 1);
    EXPECT_FALSE(cache->Get("key1").has_value());
    EXPECT_TRUE(cache->Get("key2").has_value());
}

TEST_F(LRUCacheTest, ClearOperation) {
    cache->Put("key1", 1);
    cache->Put("key2", 2);
    
    cache->Clear();
    
    EXPECT_EQ(cache->Size(), 0);
    EXPECT_FALSE(cache->Contains("key1"));
    EXPECT_FALSE(cache->Contains("key2"));
}
