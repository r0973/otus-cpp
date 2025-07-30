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

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

template <typename... Args>
struct are_all_same : std::true_type {};

template <typename T1, typename T2, typename... Args>
struct are_all_same<T1, T2, Args...>
    : std::bool_constant<std::is_same_v<T1, T2> && are_all_same<T1, Args...>::value> {};

template <typename T>
struct is_uniform_tuple : std::false_type {};

template <typename... Args>
struct is_uniform_tuple<std::tuple<Args...>> 
       : std::bool_constant<are_all_same<Args...>::value> {};

template<size_t I = 0, typename... Tp>
std::enable_if_t<I == sizeof...(Tp)>
print_tuple(const std::tuple<Tp...>&) {}

template<size_t I = 0, typename... Tp>
std::enable_if_t<I < sizeof...(Tp)>
print_tuple(const std::tuple<Tp...>& t)
{
    std::cout << std::get<I>(t);
	std::cout << (I >= 0 && I+1 != sizeof...(Tp) ? "." : "\n");
    print_tuple<I + 1>(t);
}

template<typename T>
std::enable_if_t<is_tuple<T>::value && is_uniform_tuple<T>::value>
print_ip(const T& ip_as_tuple)
{
	print_tuple(ip_as_tuple);
}