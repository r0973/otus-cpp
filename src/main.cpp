#include "lib_version.h"

#include <iostream>
#include <memory>
#include "BulkProcessor.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <bulk_size>" << std::endl;
        return 1;
    }

    std::size_t bulkSize = 0;
    try
    {
        bulkSize = static_cast<std::size_t>(std::stoul(argv[1]));
    } 
    catch (...)
    {
        std::cerr << "Wrong usage: <bulk_size> must be a positive integer" << std::endl;
        return 1;
    }
    
    BulkProcessor processor{bulkSize};

    auto consoleLogger = std::make_shared<ConsoleLogger>();
    auto fileLogger = std::make_shared<FileLogger>();

    processor.attach(consoleLogger);
    processor.attach(fileLogger);

    std::string line;
    while (std::getline(std::cin, line))
    {
        processor.ProcessCommand(Command{line});
    }
    processor.Finish();

    return 0;
}