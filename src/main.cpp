#include "lib_version.h"

#include <iostream>
#include <memory>
#include "BulkProcessor.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <bulk_size>" << std::endl;
        return 1;
    }

    try
    {
        auto bulkSize = std::stoi(argv[1]);
        
        if (bulkSize < 0)
        {
            std::cerr << "Error: <bulk_size> must be a positive integer" << std::endl;
            return 1;
        }

        BulkProcessor processor{static_cast<size_t>(bulkSize)};

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

    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}