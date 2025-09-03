/**
 * @file Rectangle.h
 * @brief \en Class for rectangle.
 *        \ru Класс для прямоугольника.
 */
#pragma once

#include "GraphicPrimitive.h"

/**
 * @class Rectangle
 * @brief \en Class for rectangle.
 *        \ru Класс для прямоугольника.
 */
class Rectangle : public GraphicPrimitive
{
public:
    /**
     * @brief \en Constructor.
     *        \ru Конструктор.
     * @param x \en X coordinate of the top-left corner.
     *          \ru Координата X верхнего левого угла.
     * @param y \en Y coordinate of the top-left corner.
     *          \ru Координата Y верхнего левого угла.
     * @param width \en Width of the rectangle.
     *              \ru Ширина прямоугольника.
     * @param height \en Height of the rectangle.
     *               \ru Высота прямоугольника.
     */
    Rectangle(double x, double y, double width, double height);

    /**
     * @brief \en Draws the rectangle.
     *        \ru Рисует прямоугольник.
     */
    void draw() const override;

private:
    double x_; ///< \en X coordinate of the top-left corner.
               ///< \ru Координата X верхнего левого угла.
    double y_; ///< \en Y coordinate of the top-left corner.
               ///< \ru Координата Y верхнего левого угла.
    double width_; ///< \en Width of the rectangle.
                    ///< \ru Ширина прямоугольника.
    double height_; ///< \en Height of the rectangle.
                     ///< \ru Высота прямоугольника.
};