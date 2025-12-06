#include "async.h"
#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <bulk_size>" << std::endl;
        return 1;
    }

    size_t bulkSize;
    try
    {
        bulkSize = static_cast<size_t>(std::stoi(argv[1]));
        if (bulkSize <= 0)
        {
            std::cerr << "Error: bulk_size must be a positive integer" << std::endl;
            return 1;
        }
    }
    catch (...)
    {
        std::cerr << "Error: bulk_size must be a positive integer" << std::endl;
        return 1;
    }

    void* context = connect(bulkSize);

    std::string line;
    while (std::getline(std::cin, line))
    {
        receive(context, line.c_str(), line.size());
    }

    disconnect(context);
    return 0;
}
