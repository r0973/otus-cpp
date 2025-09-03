#include "models/DocumentModel.h"
#include <algorithm>
#include <iostream>

DocumentModel::DocumentModel() {}

void DocumentModel::importFromFile(const std::string& filename)
{
    std::cout << "Importing document from file: " << filename << std::endl;
}

void DocumentModel::exportToFile(const std::string& filename)
{
    std::cout << "Exporting document to file: " << filename << std::endl;
}

void DocumentModel::addPrimitive(std::shared_ptr<GraphicPrimitive> primitive)
{
    primitives_.push_back(primitive);
}

void DocumentModel::removePrimitive(std::shared_ptr<GraphicPrimitive> primitive)
{
    primitives_.erase(
        std::remove_if(primitives_.begin(), primitives_.end(),
        [&primitive](const auto& ptr) { return ptr == primitive; }),
        primitives_.end()
    );
}

const std::vector<std::shared_ptr<GraphicPrimitive>>& DocumentModel::getPrimitives() const {
    return primitives_;
}
