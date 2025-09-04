#include <gtest/gtest.h>
#include "models/DocumentModel.h"
#include "primitives/Circle.h"
#include "primitives/Rectangle.h"

class DocumentModelTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        model = std::make_shared<DocumentModel>();
    }
    std::shared_ptr<DocumentModel> model;
};

TEST_F(DocumentModelTest, AddPrimitive)
{
    auto circle = std::make_shared<Circle>(10.0, 20.0, 5.0);
    model->addPrimitive(circle);
    ASSERT_EQ(model->getPrimitives().size(), 1);
}

TEST_F(DocumentModelTest, RemovePrimitive)
{
    auto circle = std::make_shared<Circle>(10.0, 20.0, 5.0);
    model->addPrimitive(circle);
    model->removePrimitive(circle);
    ASSERT_EQ(model->getPrimitives().size(), 0);
}

TEST_F(DocumentModelTest, ImportExport)
{
    testing::internal::CaptureStdout();
    model->importFromFile("test_file.txt");
    std::string importOutput = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(importOutput.find("Importing document from file: test_file.txt") != std::string::npos);

    testing::internal::CaptureStdout();
    model->exportToFile("export_file.txt");
    std::string exportOutput = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(exportOutput.find("Exporting document to file: export_file.txt") != std::string::npos);
}