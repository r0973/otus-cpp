/**
 * @file Controller.h
 * @brief \en Abstract base class for controllers.
 *        \ru Абстрактный базовый класс для контроллеров.
 */
#pragma once

#include <memory>
#include <string>

class GraphicPrimitive;

/**
 * @class Controller
 * @brief \en Abstract base class for controllers.
 *        \ru Абстрактный базовый класс для контроллеров.
 */
class Controller
{
public:
    virtual ~Controller() = default;

    /**
     * @brief \en Creates a new document.
     *        \ru Создает новый документ.
     */
    virtual void createNewDocument() = 0;

    /**
     * @brief \en Imports document from a file.
     *        \ru Импортирует документ из файла.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    virtual void importDocument(const std::string& filename) = 0;

    /**
     * @brief \en Exports document to a file.
     *        \ru Экспортирует документ в файл.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    virtual void exportDocument(const std::string& filename) = 0;

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
};
