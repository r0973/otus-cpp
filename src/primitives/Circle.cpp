#include "Circle.h"
#include <iostream>

Circle::Circle(double x, double y, double radius)
: x_{x}
, y_{y}
, radius_{radius}
{}

void Circle::draw() const
{
    std::cout << "Drawing Circle at (" << x_ << ", " << y_ << ") with radius " << radius_ << std::endl;
}
