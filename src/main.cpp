
#include <iostream>
#include <memory>

#include "models/DocumentModel.h"
#include "views/DocumentView.h"
#include "controllers/DocumentController.h"
#include "primitives/Circle.h"
#include "primitives/Rectangle.h"
#include "lib_version.h"

int main(int, char **)
{
	std::cout << "Version: " << version() << std::endl;
	
	// Создаем документ, представление и контроллер
    auto document = std::make_shared<DocumentModel>();
    auto view = std::make_shared<DocumentView>(document);
    auto controller = std::make_shared<DocumentController>(document, view);

    // Пример использования обработчиков GUI
    controller->createNewDocument();
    controller->importDocument("document.txt");
    controller->exportDocument("document_export.txt");

    // Создаем графические примитивы
    auto circle = std::make_shared<Circle>(10.0, 20.0, 5.0);
    auto rectangle = std::make_shared<Rectangle>(30.0, 40.0, 10.0, 15.0);

    // Добавляем примитивы в документ
    controller->addPrimitive(circle);
    controller->addPrimitive(rectangle);

    // Удаляем примитив из документа
    controller->removePrimitive(circle);
    controller->removePrimitive(rectangle);

    return 0;
}