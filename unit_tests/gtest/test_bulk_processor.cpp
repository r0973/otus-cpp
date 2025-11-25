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

TEST(BulkProcessor, StaticBlocks)
{
	ConsoleOutputCapture capture;
    
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
    processor.Finish();
    
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2, cmd3\nbulk: cmd4, cmd5\n";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, DynamicBlocks)
{
	ConsoleOutputCapture capture;
    
    size_t N = 3;
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);
    
    std::string line;
    std::stringstream input("cmd1\ncmd2\n{\ncmd3\ncmd4\ncmd5\n}\n{\ncmd10\ncmd11\n}\nEOF\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();
    
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2\nbulk: cmd3, cmd4, cmd5\nbulk: cmd10, cmd11\n";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, UnclosedDynamicBlock)
{
	ConsoleOutputCapture capture;
    
    size_t N = 3;
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);
    
    std::string line;
    std::stringstream input("cmd1\ncmd2\n{\ncmd3\ncmd4\ncmd11\nEOF\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();
    
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2\n";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, InclosedDynamicBlock)
{
	ConsoleOutputCapture capture;
    
    size_t N = 3;
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);
    
    std::string line;
    std::stringstream input("{\ncmd1\ncmd2\n{\ncmd3\ncmd11\n}\n}\nEOF\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();
    
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2, cmd3, cmd11\n";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, SymbolDynamicBlock)
{
	ConsoleOutputCapture capture;
    
    size_t N = 3;
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);
    
    std::string line;
    std::stringstream input("cmd1\ncmd2\n}\ncmd3\n{\ncmd11\ncmd12\n}\nEOF\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();
    
    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2, cmd3\nbulk: cmd11, cmd12\n";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, StaticBlockN0)
{
    ConsoleOutputCapture capture;
	
    size_t N = 0; // N is zero
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);

    std::string line;
    std::stringstream input("cmd1\ncmd2\ncmd3\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();

    std::string output = capture.getOutput();
    std::string expected = "";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, DynamicBlockN0)
{
    ConsoleOutputCapture capture;
	
    size_t N = 0; // N is zero
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);

    std::string line;
    std::stringstream input("{\ncmd1\ncmd2\ncmd3\n}\nEOF\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();

    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1, cmd2, cmd3\n";
    EXPECT_EQ(expected, output);
}

TEST(BulkProcessor, StaticBlockN1)
{
    ConsoleOutputCapture capture;
	
    size_t N = 1;
    BulkProcessor processor{N};
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    processor.attach(consoleLogger);

    std::string line;
    std::stringstream input("cmd1\ncmd2\ncmd3\n");
    while (std::getline(input, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();

    std::string output = capture.getOutput();
    std::string expected = "bulk: cmd1\nbulk: cmd2\nbulk: cmd3\n";
    EXPECT_EQ(expected, output);
}