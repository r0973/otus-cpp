#include "Rectangle.h"
#include <iostream>

Rectangle::Rectangle(double x, double y, double width, double height)
: x_{x}
, y_{y}
, width_{width}
, height_{height}
{

}

void Rectangle::draw() const
{
    std::cout << "Drawing Rectangle at (" << x_ << ", " << y_ << ") with width " << width_
              << " and height " << height_ << std::endl;
}
