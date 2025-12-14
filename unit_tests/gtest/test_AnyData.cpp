#include <gtest/gtest.h>
#include "AnyData.h"

using namespace nosqldb;

class AnyDataTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AnyDataTest, DefaultConstruction) {
    AnyData data;
    EXPECT_TRUE(data.Empty());
    EXPECT_EQ(data.Type(), typeid(void));
}

TEST_F(AnyDataTest, ConstructionWithValue) {
    AnyData intData(42);
    EXPECT_FALSE(intData.Empty());
    EXPECT_EQ(intData.Type(), typeid(int));
    EXPECT_TRUE(intData.Is<int>());
    EXPECT_EQ(intData.Get<int>(), 42);
    
    AnyData stringData(std::string("test"));
    EXPECT_TRUE(stringData.Is<std::string>());
    EXPECT_EQ(stringData.Get<std::string>(), "test");
}

TEST_F(AnyDataTest, TryGetMethod) {
    AnyData data(3.14);
    
    double d;
    EXPECT_TRUE(data.TryGet(d));
    EXPECT_DOUBLE_EQ(d, 3.14);
    
    int i;
    EXPECT_FALSE(data.TryGet(i));
}

TEST_F(AnyDataTest, ExceptionOnTypeMismatch) {
    AnyData data(42);
    EXPECT_THROW(data.Get<std::string>(), std::bad_cast);
}

TEST_F(AnyDataTest, MoveSemantics) {
    std::string original = "original";
    AnyData data(std::move(original));
    EXPECT_EQ(data.Get<std::string>(), "original");
}

TEST_F(AnyDataTest, CopyAndMove) {
    
    // 1. Original
    AnyData original(42);
    EXPECT_TRUE(original.Is<int>());
    EXPECT_EQ(original.Get<int>(), 42);
    
    // 2. Copy constructor
    AnyData copyConstructed(original);
    EXPECT_TRUE(copyConstructed.Is<int>());
    EXPECT_EQ(copyConstructed.Get<int>(), 42);
    
    // 3. Copy assignment
    AnyData copyAssigned;
    copyAssigned = original;
    EXPECT_TRUE(copyAssigned.Is<int>());
    EXPECT_EQ(copyAssigned.Get<int>(), 42);
    
    // 4. Move constructor
    AnyData movedFrom(100);
    AnyData moveConstructed(std::move(movedFrom));
    EXPECT_TRUE(moveConstructed.Is<int>());
    EXPECT_EQ(moveConstructed.Get<int>(), 100);
    EXPECT_TRUE(movedFrom.Empty());  // После перемещения должен быть пустым
    
    // 5. Move assignment
    AnyData moveAssigned;
    AnyData toMove(200);
    moveAssigned = std::move(toMove);
    EXPECT_TRUE(moveAssigned.Is<int>());
    EXPECT_EQ(moveAssigned.Get<int>(), 200);
    EXPECT_TRUE(toMove.Empty());
    
    std::cout << "=== Rule of Five tests PASSED ===" << std::endl;
}
