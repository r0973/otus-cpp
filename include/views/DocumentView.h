/**
 * @file DocumentView.h
 * @brief \en Class for document view.
 *        \ru Класс для представления документа.
 */
#pragma once

#include "View.h"
#include <memory>

class DocumentModel;

/**
 * @class DocumentView
 * @brief \en Class for document view.
 *        \ru Класс для представления документа.
 */
class DocumentView : public View
{
public:
    /**
     * @brief \en Constructor.
     *        \ru Конструктор.
     * @param document \en Pointer to the document model.
     *                  \ru Указатель на модель документа.
     */
    DocumentView(std::shared_ptr<DocumentModel> document);

    /**
     * @brief \en Displays the document.
     *        \ru Отображает документ.
     */
    void display() const override;

private:
    std::shared_ptr<DocumentModel> document_; ///< \en Pointer to the document model.
                                              ///< \ru Указатель на модель документа.
};