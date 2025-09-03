/**
 * @file Circle.h
 * @brief \en Class for circle.
 *        \ru Класс для круга.
 */
#pragma once

#include "GraphicPrimitive.h"

/**
 * @class Circle
 * @brief \en Class for circle.
 *        \ru Класс для круга.
 */
class Circle : public GraphicPrimitive
{
public:
    /**
     * @brief \en Constructor.
     *        \ru Конструктор.
     * @param x \en X coordinate of the center.
     *          \ru Координата X центра.
     * @param y \en Y coordinate of the center.
     *          \ru Координата Y центра.
     * @param radius \en Radius of the circle.
     *               \ru Радиус круга.
     */
    Circle(double x, double y, double radius);

    /**
     * @brief \en Draws the circle.
     *        \ru Рисует круг.
     */
    void draw() const override;

private:
    double x_; ///< \en X coordinate of the center.
               ///< \ru Координата X центра.
    double y_; ///< \en Y coordinate of the center.
               ///< \ru Координата Y центра.
    double radius_; ///< \en Radius of the circle.
                    ///< \ru Радиус круга.
};