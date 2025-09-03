#include "views/DocumentView.h"
#include "models/DocumentModel.h"
#include "primitives/GraphicPrimitive.h"
#include <iostream>


DocumentView::DocumentView(std::shared_ptr<DocumentModel> document)
: document_{document}
{}

void DocumentView::display() const
{
    std::cout << "Displaying document:" << std::endl;
    for (const auto& primitive : document_->getPrimitives())
	{
        primitive->draw();
    }
}
