/**
 * @file View.h
 * @brief \en Abstract base class for views.
 *        \ru Абстрактный базовый класс для представлений.
 */
#pragma once

/**
 * @class View
 * @brief \en Abstract base class for views.
 *        \ru Абстрактный базовый класс для представлений.
 */
class View
{
public:
    virtual ~View() = default;

    /**
     * @brief \en Displays the model.
     *        \ru Отображает модель.
     */
    virtual void display() const = 0;
};