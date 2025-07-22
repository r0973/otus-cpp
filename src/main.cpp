#include "lib.h"

#include <iostream>

int main(int , char **)
{
	try
	{
		auto ip_pool = read_ip4_pool(std::cin);
		reverse_sort(std::begin(ip_pool), std::end(ip_pool));
		std::cout << ip_pool;
		std::cout << filter(ip_pool, {0, 1});
		std::cout << filter(ip_pool, {0, 46}, {1, 70});
		std::cout << filter(ip_pool, 46);
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

    return 0;
} 
