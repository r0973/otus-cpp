#include "lib_version.h"

#include <iostream>
#include <vector>
#include <map>
#include "memory_allocator.h"
#include "custom_container.h"

constexpr int factorial(int n)
{
    return (n == 0) ? 1 : n * factorial(n - 1);
}

int main(int, char **)
{
	std::cout << "Version: " << version() << std::endl;
	
	// map with standart allocator
	std::map<int, int> std_alloc_map;
    for (std::size_t i = 0; i < 10; ++i)
		std_alloc_map[i] = factorial(i);

	for (const auto& [key, value] : std_alloc_map)
        std::cout << key << " " << value << std::endl;
    std::cout << std::endl;

    // map with custom allocator
	using MapAllocator = MemoryAllocator<std::pair<const int, int>, 10>;
	std::map<int, int, std::less<int>, MapAllocator> cust_alloc_map;
    for (std::size_t i = 0; i < 10; ++i)
        cust_alloc_map[i] = factorial(i);

    for (const auto& [key, value] : cust_alloc_map)
        std::cout << key << " " << value << std::endl;
    std::cout << std::endl;

	// vector with custom allocator
	std::vector<int, MemoryAllocator<int,100>> vec;
	
	for( int i = 0; i < 10; ++i)
			vec.push_back(i);
	
	for( auto&& v : vec )
		std::cout << v << " ";
	std::cout << std::endl; 

	// custom container with standart allocator
	CustomContainer<int> std_alloc_container;
	for (int i = 0; i < 10; ++i)
        std_alloc_container.push_back(i);

	for (auto i : std_alloc_container)
        std::cout << i << " ";
	std::cout << std::endl; 

	return 0;
}