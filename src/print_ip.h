#pragma once

#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include <algorithm>

template<typename T>
struct is_container :
	std::bool_constant<std::is_same_v<T, std::vector<int>> || 
    	               std::is_same_v<T, std::list<short>>> {};

template<typename T>
inline constexpr bool is_container_v = is_container<T>::value;

template<typename T>
struct is_tuple : std::false_type {};

template<typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

template<typename T>
inline constexpr bool is_tuple_v = is_tuple<T>::value;

template<typename... Args>
struct are_all_same : std::true_type {};

template<typename T1, typename T2, typename... Args>
struct are_all_same<T1, T2, Args...>
    : std::bool_constant<std::is_same_v<T1, T2> && are_all_same<T1, Args...>::value> {};

template <typename... Args>
inline constexpr bool are_all_same_v = are_all_same<Args...>::value;
template<typename T>
struct is_uniform_tuple : std::false_type {};

template<typename... Args>
struct is_uniform_tuple<std::tuple<Args...>> 
       : std::bool_constant<are_all_same_v<Args...>> {};

template<typename... Args>
inline constexpr bool is_uniform_tuple_v = is_uniform_tuple<Args...>::value;


namespace detail
{

template<size_t I = 0, typename... Tp>
std::enable_if_t<I == sizeof...(Tp)>
print_tuple(const std::tuple<Tp...>&) {}

template<size_t I = 0, typename... Tp>
std::enable_if_t<I < sizeof...(Tp)>
print_tuple(const std::tuple<Tp...>& t)
{
    std::cout << std::get<I>(t);
	std::cout << (I >= 0 && I+1 != sizeof...(Tp) ? "." : "");
    print_tuple<I + 1>(t);
}

}// end namespace detail

template<typename T>
std::enable_if_t<is_container_v<T>, std::ostream&>
operator << (std::ostream& os, const T& cont)
{
	if (!cont.empty())
	{
		std::for_each(std::cbegin(cont), std::prev(std::cend(cont)),
			[&os](const auto& val) { os << val << "."; });
		os << cont.back();
	}
	return os;
}

template<typename T>
std::enable_if_t<std::is_integral_v<T>>
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
    std::cout << "\n";
}

template<typename T>
std::enable_if_t<std::is_same_v<T, std::string>>
print_ip(const T& ip_as_string)
{
	std::cout << ip_as_string << "\n";
}

template<typename T>
std::enable_if_t<is_container_v<T>>
print_ip(const T& ip_as_cont)
{
	std::cout << ip_as_cont << "\n";
}

template<typename T>
std::enable_if_t<is_tuple_v<T> && is_uniform_tuple_v<T>>
print_ip(const T& ip_as_tuple)
{
	detail::print_tuple(ip_as_tuple);
	std::cout << "\n";
}