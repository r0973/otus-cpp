#pragma once

#include <type_traits>
#include <vector>
#include <list>
#include <iostream>

template <typename T>
std::enable_if_t<std::is_integral<T>::value>
print_ip(const T& ip_as_int)
{
    const size_t size = sizeof(T);
    
    for (size_t i = 0; i < size; ++i)
	{
        if (i != 0)
		{
            std::cout << ".";
        }
        int shift = 8 * (size - 1 - i);
        std::cout << static_cast<int>((ip_as_int >> shift) & 0xFF);
    }
    std::cout << std::endl;
}

template<typename T>
std::enable_if_t<std::is_same<T, std::string>::value>
print_ip(const T& ip_as_string)
{
	std::cout << ip_as_string << std::endl;
}

template<typename T>
std::enable_if_t<std::is_same<T, std::vector<int>>::value || 
                 std::is_same<T, std::list<short>>::value,
				 std::ostream&>
operator << (std::ostream& os, const T& container)
{
	if (!container.empty())
	{
		auto first = std::cbegin(container);
		auto last = std::cend(container);
		for (; first != last; ++first)
		{
			if (first != std::cbegin(container))
			{
				os << ".";
			}
			os << *first;
		}
		os << std::endl;
	}
	return os;
};

template<typename T>
std::enable_if_t<std::is_same<T, std::vector<int>>::value>
print_ip(const T& ip_as_vector)
{
	std::cout << ip_as_vector; 
}

template<typename T>
std::enable_if_t<std::is_same<T,std::list<short>>::value>
print_ip(const T& ip_as_list)
{
	std::cout << ip_as_list;
}