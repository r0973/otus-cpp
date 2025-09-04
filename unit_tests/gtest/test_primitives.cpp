#include <gtest/gtest.h>
#include "primitives/Circle.h"
#include "primitives/Rectangle.h"

TEST(CircleTest, Draw) {
    Circle circle(10.0, 20.0, 5.0);
    testing::internal::CaptureStdout();
    circle.draw();
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Drawing Circle at (10, 20) with radius 5") != std::string::npos);
}

TEST(RectangleTest, Draw) {
    Rectangle rectangle(10.0, 20.0, 5.0, 6.0);
    testing::internal::CaptureStdout();
    rectangle.draw();
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Drawing Rectangle at (10, 20) with width 5 and height 6") != std::string::npos);
}