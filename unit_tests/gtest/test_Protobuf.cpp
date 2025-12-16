#include <gtest/gtest.h>
#include "AnyData.h"
#include "StorageConfig.h"
#include "NoSQLDataBase.h"
#include "protos/user.pb.h"
#include "protos/product.pb.h"

using namespace nosqldb;
using namespace nosqldb::data;

class ProtobufSerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 
    }
    
    void TearDown() override {
        // Очистка не требуется
    }
};

// Тест для базовых типов (int, double, string, bool)
TEST_F(ProtobufSerializationTest, BasicTypesRoundTrip) {
    // 1. Int
    {
        AnyData intData(42);
        std::vector<char> serialized = intData.Serialize();
        EXPECT_FALSE(serialized.empty());
        
        AnyData deserialized = AnyData::Deserialize(serialized);
        EXPECT_TRUE(deserialized.Is<int>());
        EXPECT_EQ(deserialized.Get<int>(), 42);
    }
    
    // 2. Double
    {
        AnyData doubleData(3.14159);
        std::vector<char> serialized = doubleData.Serialize();
        
        AnyData deserialized = AnyData::Deserialize(serialized);
        EXPECT_TRUE(deserialized.Is<double>());
        EXPECT_DOUBLE_EQ(deserialized.Get<double>(), 3.14159);
    }
    
    // 3. String
    {
        AnyData stringData(std::string("Hello Protobuf"));
        std::vector<char> serialized = stringData.Serialize();
        
        AnyData deserialized = AnyData::Deserialize(serialized);
        EXPECT_TRUE(deserialized.Is<std::string>());
        EXPECT_EQ(deserialized.Get<std::string>(), "Hello Protobuf");
    }
    
    // 4. Bool
    {
        AnyData boolData(true);
        std::vector<char> serialized = boolData.Serialize();
        
        AnyData deserialized = AnyData::Deserialize(serialized);
        EXPECT_TRUE(deserialized.Is<bool>());
        EXPECT_EQ(deserialized.Get<bool>(), true);
    }
}

// Тест для Protobuf типа User
TEST_F(ProtobufSerializationTest, UserSerializationRoundTrip) {
    User originalUser;
    originalUser.set_id(99);
    originalUser.set_name("Bob Test");
    originalUser.set_email("bob@test.com");
    originalUser.set_age(45);
    originalUser.set_city("Oslo");

    // 1. Оборачиваем в AnyData
    AnyData anyData(originalUser);
    
    // 2. Сериализуем через protobuf
    std::vector<char> serializedData = anyData.Serialize();
    EXPECT_FALSE(serializedData.empty());

    // 3. Десериализуем обратно (автоопределение типа из protobuf)
    AnyData deserializedAny = AnyData::Deserialize(serializedData);
    
    // 4. Проверяем тип
    EXPECT_TRUE(deserializedAny.Is<User>());
    
    // 5. Извлекаем и проверяем данные
    User retrievedUser = deserializedAny.Get<User>();
    EXPECT_EQ(originalUser.id(), retrievedUser.id());
    EXPECT_EQ(originalUser.name(), retrievedUser.name());
    EXPECT_EQ(originalUser.email(), retrievedUser.email());
    EXPECT_EQ(originalUser.age(), retrievedUser.age());
    EXPECT_EQ(originalUser.city(), retrievedUser.city());
}

// Тест для Protobuf типа Product
TEST_F(ProtobufSerializationTest, ProductSerializationRoundTrip) {
    Product originalProduct;
    originalProduct.set_product_id(123);
    originalProduct.set_name("Laptop");
    originalProduct.set_category("Electronics");
    originalProduct.set_stock(42); 
    originalProduct.set_price(999.99);

    AnyData anyData(originalProduct);
    std::vector<char> serializedData = anyData.Serialize();
    
    AnyData deserializedAny = AnyData::Deserialize(serializedData);
    EXPECT_TRUE(deserializedAny.Is<Product>());
    
    Product retrievedProduct = deserializedAny.Get<Product>();
    EXPECT_EQ(originalProduct.product_id(), retrievedProduct.product_id());
    EXPECT_EQ(originalProduct.name(), retrievedProduct.name());
    EXPECT_EQ(originalProduct.category(), retrievedProduct.category());
    EXPECT_EQ(originalProduct.stock(), retrievedProduct.stock()); 
    EXPECT_DOUBLE_EQ(originalProduct.price(), retrievedProduct.price());
}

// Тест смешанных типов в хранилище
TEST_F(ProtobufSerializationTest, MixedTypesInStorage) {
    StorageConfig config;
    NoSQLDataBase db(config);
    
    // Сохраняем разные типы
    EXPECT_TRUE(db.Put("int_key", 100));
    EXPECT_TRUE(db.Put("string_key", std::string("test")));
    
    User user;
    user.set_id(1);
    user.set_name("Alice");
    EXPECT_TRUE(db.Put("user_key", user));
    
    Product product;
    product.set_product_id(2);
    product.set_name("Phone");
    EXPECT_TRUE(db.Put("product_key", product));
    
    // Получаем и проверяем
    auto intVal = db.Get<int>("int_key");
    EXPECT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 100);
    
    auto strVal = db.Get<std::string>("string_key");
    EXPECT_TRUE(strVal.has_value());
    EXPECT_EQ(*strVal, "test");
    
    auto userVal = db.Get<User>("user_key");
    EXPECT_TRUE(userVal.has_value());
    EXPECT_EQ(userVal->id(), 1);
    EXPECT_EQ(userVal->name(), "Alice");
    
    auto productVal = db.Get<Product>("product_key");
    EXPECT_TRUE(productVal.has_value());
    EXPECT_EQ(productVal->product_id(), 2);
    EXPECT_EQ(productVal->name(), "Phone");
}

// Тест на пустые/неправильные данные
TEST_F(ProtobufSerializationTest, InvalidDataHandling) {
    // Пустые данные
    std::vector<char> emptyData;
    EXPECT_THROW(AnyData::Deserialize(emptyData), std::runtime_error);
    
    // Мусорные данные
    std::vector<char> garbageData = {0x01, 0x02, 0x03, 0x04};
    EXPECT_THROW(AnyData::Deserialize(garbageData), std::runtime_error);
    
    // Неполные protobuf данные
    std::vector<char> truncatedData = {0x0A, 0x03, 0x66, 0x6F, 0x6F};
    EXPECT_THROW(AnyData::Deserialize(truncatedData), std::runtime_error);
}

// Небольшой тест производительности
TEST_F(ProtobufSerializationTest, PerformanceTest) {
    const int ITERATIONS = 1000;
    
    User user;
    user.set_id(1);
    user.set_name("Test User");
    user.set_email("test@example.com");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; ++i) {
        AnyData data(user);
        auto serialized = data.Serialize();
        auto deserialized = AnyData::Deserialize(serialized);
        EXPECT_TRUE(deserialized.Is<User>());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Protobuf serialization/deserialization of " << ITERATIONS 
              << " User objects took " << duration.count() << "ms" << std::endl;
    std::cout << "Average per operation: " 
              << (duration.count() * 1000.0 / ITERATIONS) << "μs" << std::endl;
}
