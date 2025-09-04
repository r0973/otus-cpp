#include <gtest/gtest.h>
#include "views/DocumentView.h"
#include "models/DocumentModel.h"
#include "primitives/Circle.h"

class DocumentViewTest : public ::testing::Test
{
protected:
    void SetUp() override
	{
        model = std::make_shared<DocumentModel>();
        view = std::make_shared<DocumentView>(model);
    }
    std::shared_ptr<DocumentModel> model;
    std::shared_ptr<DocumentView> view;
};

TEST_F(DocumentViewTest, Display)
{
    auto circle = std::make_shared<Circle>(10.0, 20.0, 5.0);
    model->addPrimitive(circle);

    testing::internal::CaptureStdout();
    view->display();
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.find("Displaying document:") != std::string::npos);
}
