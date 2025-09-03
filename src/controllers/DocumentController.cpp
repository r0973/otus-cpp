#include "controllers/DocumentController.h"
#include "models/DocumentModel.h"
#include "views/DocumentView.h"
#include "primitives/GraphicPrimitive.h"
#include <iostream>

DocumentController::DocumentController(std::shared_ptr<DocumentModel> document, std::shared_ptr<DocumentView> view)
: document_{document}
, view_{view}
{}

void DocumentController::createNewDocument()
{
    document_ = std::make_shared<DocumentModel>();
    view_ = std::make_shared<DocumentView>(document_);
    std::cout << "New document created." << std::endl;
}

void DocumentController::importDocument(const std::string& filename)
{
    document_->importFromFile(filename);
    view_->display();
}

void DocumentController::exportDocument(const std::string& filename)
{
    document_->exportToFile(filename);
}

void DocumentController::addPrimitive(std::shared_ptr<GraphicPrimitive> primitive)
{
    document_->addPrimitive(primitive);
    view_->display();
}

void DocumentController::removePrimitive(std::shared_ptr<GraphicPrimitive> primitive)
{
    document_->removePrimitive(primitive);
    view_->display();
}
