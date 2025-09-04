/**
 * @file DocumentController.h
 * @brief \en Class for document controller.
 *        \ru Класс для контроллера документа.
 */
#pragma once

#include "Controller.h"
#include <memory>

class DocumentModel;
class DocumentView;

/**
 * @class DocumentController
 * @brief \en Class for document controller.
 *        \ru Класс для контроллера документа.
 */
class DocumentController : public Controller
{
public:
    /**
     * @brief \en Constructor.
     *        \ru Конструктор.
     * @param document \en Pointer to the document model.
     *                  \ru Указатель на модель документа.
     * @param view \en Pointer to the document view.
     *              \ru Указатель на представление документа.
     */
    DocumentController(std::shared_ptr<DocumentModel> document, std::shared_ptr<DocumentView> view);

    /**
     * @brief \en Creates a new document.
     *        \ru Создает новый документ.
     */
    void createNewDocument() override;

    /**
     * @brief \en Imports document from a file.
     *        \ru Импортирует документ из файла.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    void importDocument(const std::string& filename) override;

    /**
     * @brief \en Exports document to a file.
     *        \ru Экспортирует документ в файл.
     * @param filename \en File name.
     *                  \ru Имя файла.
     */
    void exportDocument(const std::string& filename) override;

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

private:
    std::shared_ptr<DocumentModel> document_; ///< \en Pointer to the document model.
                                               ///< \ru Указатель на модель документа.
    std::shared_ptr<DocumentView> view_;     ///< \en Pointer to the document view.
                                               ///< \ru Указатель на представление документа.
};