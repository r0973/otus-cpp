/**
 * @file Model.h
 * @brief \en Abstract base class for models.
 *        \ru Абстрактный базовый класс для моделей.
 */
#pragma once

#include <memory>
#include <vector>
#include <string>

class GraphicPrimitive;

/**
 * @class Model
 * @brief \en Abstract base class for models.
 *        \ru Абстрактный базовый класс для моделей.
 */
class Model
{
public:
    virtual ~Model() = default;

    /**
     * @brief \en Imports document from a file.
     *        \ru Импортирует документ из файла.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    virtual void importFromFile(const std::string& filename) = 0;

    /**
     * @brief \en Exports document to a file.
     *        \ru Экспортирует документ в файл.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    virtual void exportToFile(const std::string& filename) = 0;

    /**
     * @brief \en Adds a graphic primitive to the document.
     *        \ru Добавляет графический примитив в документ.
     * @param primitive \en Pointer to the graphic primitive.
     *                     \ru Указатель на графический примитив.
     */
    virtual void addPrimitive(std::shared_ptr<GraphicPrimitive> primitive) = 0;

    /**
     * @brief \en Removes a graphic primitive from the document.
     *        \ru Удаляет графический примитив из документа.
     * @param primitive \en Pointer to the graphic primitive.
     *                     \ru Указатель на графический примитив.
     */
    virtual void removePrimitive(std::shared_ptr<GraphicPrimitive> primitive) = 0;

    /**
     * @brief \en Gets the list of graphic primitives.
     *        \ru Получает список графических примитивов.
     * @return \en Vector of pointers to graphic primitives.
     *         \ru Вектор указателей на графические примитивы.
     */
    virtual const std::vector<std::shared_ptr<GraphicPrimitive>>& getPrimitives() const = 0;
};