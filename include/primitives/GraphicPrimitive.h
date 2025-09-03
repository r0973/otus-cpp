/**
 * @file GraphicPrimitive.h
 * @brief \en Base class for graphic primitives.
 *        \ru Базовый класс для графических примитивов.
 */
#pragma once

#include <memory>

/**
 * @class GraphicPrimitive
 * @brief \en Base class for all graphic primitives.
 *        \ru Базовый класс для всех графических примитивов.
 */
class GraphicPrimitive
{
public:
    virtual ~GraphicPrimitive() = default;

    /**
     * @brief \en Draws the graphic primitive.
     *        \ru Рисует графический примитив.
     */
    virtual void draw() const = 0;
};
