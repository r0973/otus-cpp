#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cmath>
#include "NoSQLDataBase.h"
#include "TypeRegistry.h"
#include "AnyData.h"

using namespace nosqldb;

class SerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        TypeRegistry::GetInstance().RegisterStandardTypes();
    }

    void TearDown() override {
        // Очистка не требуется
    }
};

// Тест сериализации и десериализации базовых типов
TEST_F(SerializationTest, BasicTypeSerialization) {
    // 1. int
    AnyData originalInt(42);
    std::vector<char> serializedInt = originalInt.Serialize();
    AnyData deserializedInt = AnyData::Deserialize(serializedInt);
    
    EXPECT_TRUE(deserializedInt.Is<int>());
    EXPECT_EQ(deserializedInt.Get<int>(), 42);

    // 2. double
    AnyData originalDouble(3.14159);
    std::vector<char> serializedDouble = originalDouble.Serialize();
    AnyData deserializedDouble = AnyData::Deserialize(serializedDouble);
    
    EXPECT_TRUE(deserializedDouble.Is<double>());
    EXPECT_NEAR(deserializedDouble.Get<double>(), 3.14159, 0.00001);

    // 3. std::string
    AnyData originalString(std::string("Hello, World!"));
    std::vector<char> serializedString = originalString.Serialize();
    AnyData deserializedString = AnyData::Deserialize(serializedString);
    
    EXPECT_TRUE(deserializedString.Is<std::string>());
    EXPECT_EQ(deserializedString.Get<std::string>(), "Hello, World!");
}

// Тест на ошибку при попытке десериализовать неизвестный тип
TEST_F(SerializationTest, UnknownTypeDeserializationError) {
    // Создаем "битый" буфер с несуществующим ID типа (например, 999)
    std::vector<char> badData = {0x00, 0x00, 0x00, 0x00, /* Payload for ID 0 */}; 
    // Заменяем первые 4 байта на 999
    uint32_t badId = 999;
    std::memcpy(badData.data(), &badId, sizeof(badId));

    EXPECT_THROW(AnyData::Deserialize(badData), std::runtime_error); 
}
