#include <gtest/gtest.h>
#include "Command.h"

TEST(Command, CommandTypes)
{
    EXPECT_TRUE(Command("{").isBlockStart());
    EXPECT_TRUE(Command("}").isBlockEnd());
    EXPECT_TRUE(Command("EOF").isEOF());
    EXPECT_TRUE(Command("{").isControlCommand());
    EXPECT_TRUE(Command("cmd1").shouldBeProcessed());
    EXPECT_FALSE(Command("").shouldBeProcessed());
}