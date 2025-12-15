// unit_tests/gtest/test_Protobuf.cpp

#include <gtest/gtest.h>
#include "AnyData.h"
#include "TypeRegistry.h"
#include "protos/user.pb.h"
#include "protos/product.pb.h"

using namespace nosqldb;
using namespace nosqldb::data;

class ProtobufSerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Убедимся, что типы зарегистрированы перед тестами
        TypeRegistry::GetInstance().RegisterStandardTypes();
    }
};

TEST_F(ProtobufSerializationTest, UserSerializationRoundTrip){
    User originalUser;
    originalUser.set_id(99);
    originalUser.set_name("Bob Test");
    originalUser.set_email("bob@test.com");
    originalUser.set_age(45);
    originalUser.set_city("Oslo");

    // 1. Оборачиваем в AnyData
    AnyData anyData(originalUser);
    
    // 2. Сериализуем (используется BinarySerializer<User> -> Protobuf Serialize)
    std::vector<char> serializedData = anyData.Serialize();
    
    // Проверяем, что бинарные данные не пустые
    EXPECT_FALSE(serializedData.empty());

    // 3. Десериализуем обратно
    AnyData deserializedAny = AnyData::Deserialize(serializedData);
    
    // 4. Проверяем тип и извлекаем объект User
    EXPECT_TRUE(deserializedAny.Is<User>());
    User retrievedUser = deserializedAny.Get<User>();

    // 5. Проверяем целостность данных
    EXPECT_EQ(originalUser.id(), retrievedUser.id());
    EXPECT_EQ(originalUser.name(), retrievedUser.name());
    EXPECT_EQ(originalUser.email(), retrievedUser.email());
    EXPECT_EQ(originalUser.age(), retrievedUser.age());
    EXPECT_EQ(originalUser.city(), retrievedUser.city());
}
