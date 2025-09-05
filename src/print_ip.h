/**
 * @file print_ip.h
 * @brief IP address printing utilities
 * @details Utility functions for printing IP addresses, implemented for various 
 * input parameters using the SFINAE mechanism.
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include <algorithm>

/**
 * @brief Type trait to check if type is a supported container (std::vector<int> or std::list<short>)
 * @tparam T Type to check
 */
template<typename T>
struct is_container :
	std::bool_constant<std::is_same_v<T, std::vector<int>> || 
    	               std::is_same_v<T, std::list<short>>> {};

/// @brief Helper variable template for is_container
template<typename T>
inline constexpr bool is_container_v = is_container<T>::value;

/**
 * @brief Type trait to check if type is a std::tuple
 * @tparam T Type to check
 */
template<typename T>
struct is_tuple : std::false_type {};

/// @brief Specialization for std::tuple
template<typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

/// @brief Helper variable template for is_tuple
template<typename T>
inline constexpr bool is_tuple_v = is_tuple<T>::value;

/**
 * @brief Type trait to check if all types in parameter pack are the same
 * @tparam Args Types to check
 */
template<typename... Args>
struct are_all_same : std::true_type {};

/// @brief Recursive case for are_all_same
template<typename T1, typename T2, typename... Args>
struct are_all_same<T1, T2, Args...>
    : std::bool_constant<std::is_same_v<T1, T2> && are_all_same<T1, Args...>::value> {};

/// @brief Helper variable template for are_all_same
template <typename... Args>
inline constexpr bool are_all_same_v = are_all_same<Args...>::value;

/**
 * @brief Type trait to check if tuple contains elements of the same type
 * @tparam T Type to check (should be std::tuple)
 */
template<typename T>
struct is_uniform_tuple : std::false_type {};

/// @brief Specialization for uniform tuples
template<typename... Args>
struct is_uniform_tuple<std::tuple<Args...>> 
       : std::bool_constant<are_all_same_v<Args...>> {};

/// @brief Helper variable template for is_uniform_tuple
template<typename... Args>
inline constexpr bool is_uniform_tuple_v = is_uniform_tuple<Args...>::value;


namespace detail
{

/**
 * @brief Helper function to print a tuple recursively.
 * @tparam I Index of the current element in the tuple.
 * @tparam Tp Types of the elements in the tuple.
 * @param t The tuple to print.
 * @details This function is a helper for printing tuples element by element.
 */
template<size_t I = 0, typename... Tp>
std::enable_if_t<I == sizeof...(Tp)>
print_tuple(const std::tuple<Tp...>&) {}

/**
 * @brief Helper function to print a tuple recursively.
 * @tparam I Index of the current element in the tuple.
 * @tparam Tp Types of the elements in the tuple.
 * @param t The tuple to print.
 * @details This function is a helper for printing tuples element by element.
 */
template<size_t I = 0, typename... Tp>
std::enable_if_t<I < sizeof...(Tp)>
print_tuple(const std::tuple<Tp...>& t)
{
    std::cout << std::get<I>(t);
	std::cout << (I >= 0 && I+1 != sizeof...(Tp) ? "." : "");
    print_tuple<I + 1>(t);
}

}// end namespace detail

/**
 * @brief Overload of the output stream operator for container types.
 * @tparam T The container type.
 * @param os The output stream.
 * @param cont The container to print.
 * @return std::ostream& The output stream.
 */
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

/**
 * @brief Prints an IP address represented as an integral type.
 * @tparam T The integral type representing the IP address.
 * @param ip_as_int The IP address as an integral type.
 * @details The address can be represented as an arbitrary integer type.
 * Outputs byte by byte in unsigned form, starting from the most significant byte, 
 * with a dot (.) as a separator. All bytes of the number are output.
 */
template<typename T>
std::enable_if_t<std::is_integral_v<T>>
print_ip(const T& ip_as_int)
{
    const size_t size = sizeof(T);
    for (size_t i = 0; i < size; ++i)
	{
        int shift = 8 * (size - 1 - i);
        std::cout << (i != 0 ? "." : "") << static_cast<int>((ip_as_int >> shift) & 0xFF);
    }
    std::cout << "\n";
}

/**
 * @brief Prints an IP address represented as a string.
 * @tparam T The string type representing the IP address.
 * @param ip_as_string The IP address as a string.
 * @details The address is represented as a string. Outputs as is, regardless of the content.
 */
template<typename T>
std::enable_if_t<std::is_same_v<T, std::string>>
print_ip(const T& ip_as_string)
{
	std::cout << ip_as_string << "\n";
}

/**
 * @brief Prints an IP address represented as a container.
 * @tparam T The container type representing the IP address.
 * @param ip_as_cont The IP address as a container.
 * @details The address can be represented as containers like std::list, std::vector. 
 * Outputs the full content of the container element by element, separated by a dot (.). Elements are output as is.
 */
template<typename T>
std::enable_if_t<is_container_v<T>>
print_ip(const T& ip_as_cont)
{
	std::cout << ip_as_cont << "\n";
}

/**
 * @brief Prints an IP address represented as a uniform tuple.
 * @tparam T The tuple type representing the IP address.
 * @param ip_as_tuple The IP address as a tuple.
 * @details The address can be represented as std::tuple provided all types are the same. Outputs the full content element by element, separated by a dot (.). Elements are output as is. If the tuple types are not the same, a compilation error should occur.
 */
template<typename T>
std::enable_if_t<is_tuple_v<T> && is_uniform_tuple_v<T>>
print_ip(const T& ip_as_tuple)
{
	detail::print_tuple(ip_as_tuple);
	std::cout << "\n";
}