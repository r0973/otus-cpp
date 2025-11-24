#include <gtest/gtest.h>
#include "BulkProcessor.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

class ConsoleOutputCapture
{
public:
    ConsoleOutputCapture()
    {
        old_buf = std::cout.rdbuf(buffer.rdbuf());
    }
public:
    ~ConsoleOutputCapture()
    {
        std::cout.rdbuf(old_buf);
    }
public:
    std::string getOutput() const
    {
        return buffer.str();
    }
public:
    void clear()
    {
        buffer.str("");
        buffer.clear();
    }
private:
    std::stringstream buffer;
    std::streambuf* old_buf;
};

TEST(BulkProcessor, String)
{
    ConsoleOutputCapture capture;
	{
        size_t N = 3;
        BulkProcessor processor{N};

        auto consoleLogger = std::make_shared<ConsoleLogger>();
        processor.attach(consoleLogger);
        
    
        std::string line;
        std::stringstream input("{\ncmd1\ncmd2\ncmd3\n}\nEOF\n");
        while (std::getline(input, line))
        {
            processor.ProcessCommand(Command{line});
        }
    }
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2, cmd3\n";
    EXPECT_EQ(output, expected);
}

TEST(BulkProcessor, StaticBlocks)
{
	ConsoleOutputCapture capture;
    {
        size_t N = 3;
        BulkProcessor processor{N};
        auto consoleLogger = std::make_shared<ConsoleLogger>();
        processor.attach(consoleLogger);
        
        std::string line;
        std::stringstream input("cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n");
        while (std::getline(input, line))
        {
            processor.ProcessCommand(Command{line});
        }
    } // ← Деструктор вызывается здесь
    
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2, cmd3\nbulk: cmd4, cmd5\n";
    EXPECT_EQ(output, expected);
}