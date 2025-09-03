/**
 * @file DocumentModel.h
 * @brief \en Class for document model.
 *        \ru Класс для модели документа.
 */
#pragma once

#include "Model.h"
#include <vector>
#include <string>

class GraphicPrimitive;

/**
 * @class DocumentModel
 * @brief \en Class for document model.
 *        \ru Класс для модели документа.
 */
class DocumentModel : public Model
{
public:
    /**
     * @brief \en Constructor.
     *        \ru Конструктор.
     */
    DocumentModel();

    /**
     * @brief \en Imports document from a file.
     *        \ru Импортирует документ из файла.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    void importFromFile(const std::string& filename) override;

    /**
     * @brief \en Exports document to a file.
     *        \ru Экспортирует документ в файл.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    void exportToFile(const std::string& filename) override;

    /**
     * @brief \en Adds a graphic primitive to the document.
     *        \ru Добавляет графический примитив в документ.
     * @param primitive \en Pointer to the graphic primitive.
     *                     \ru Указатель на графический примитив.
     */
    void addPrimitive(std::shared_ptr<GraphicPrimitive> primitive) override;

    /**
     * @brief \en Removes a graphic primitive from the document.
     *        \ru Удаляет графический примитив из документа.
     * @param primitive \en Pointer to the graphic primitive.
     *                     \ru Указатель на графический примитив.
     */
    void removePrimitive(std::shared_ptr<GraphicPrimitive> primitive) override;

    /**
     * @brief \en Gets the list of graphic primitives.
     *        \ru Получает список графических примитивов.
     * @return \en Vector of pointers to graphic primitives.
     *         \ru Вектор указателей на графические примитивы.
     */
    const std::vector<std::shared_ptr<GraphicPrimitive>>& getPrimitives() const override;

private:
    std::vector<std::shared_ptr<GraphicPrimitive>> primitives_; ///< \en List of graphic primitives.
                                                                 ///< \ru Список графических примитивов.
};
