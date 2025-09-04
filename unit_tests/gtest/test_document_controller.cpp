#include <gtest/gtest.h>
#include "controllers/DocumentController.h"
#include "models/DocumentModel.h"
#include "views/DocumentView.h"
#include "primitives/Circle.h"

class DocumentControllerTest : public ::testing::Test
{
protected:
    void SetUp() override
	{
        model = std::make_shared<DocumentModel>();
        view = std::make_shared<DocumentView>(model);
        controller = std::make_shared<DocumentController>(model, view);
    }
    std::shared_ptr<DocumentModel> model;
    std::shared_ptr<DocumentView> view;
    std::shared_ptr<DocumentController> controller;
};

TEST_F(DocumentControllerTest, CreateNewDocument)
{
    testing::internal::CaptureStdout();
    controller->createNewDocument();
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("New document created.") != std::string::npos);
}

TEST_F(DocumentControllerTest, ImportDocument)
{
    testing::internal::CaptureStdout();
    controller->importDocument("test_file.txt");
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Importing document from file: test_file.txt") != std::string::npos);
}

TEST_F(DocumentControllerTest, AddPrimitive)
{
    auto circle = std::make_shared<Circle>(10.0, 20.0, 5.0);
    testing::internal::CaptureStdout();
    controller->addPrimitive(circle);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Drawing Circle at (10, 20) with radius 5") != std::string::npos);
}

TEST_F(DocumentControllerTest, RemovePrimitive)
{
    auto circle = std::make_shared<Circle>(10.0, 20.0, 5.0);
    controller->addPrimitive(circle);
    testing::internal::CaptureStdout();
    controller->removePrimitive(circle);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Displaying document:") != std::string::npos);
}
