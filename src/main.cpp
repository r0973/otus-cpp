#include "lib_version.h"
#include "print_ip.h"
#include <cstdint>

int main(int, char **) {
	print_ip( int8_t{-1} ); // 255 
	print_ip( int16_t{0} ); // 0.0 
	print_ip( int32_t{2130706433} ); // 127.0.0.1 
	print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41 
	print_ip( std::string{"Hello, World!"} ); // Hello, World! 
	print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400 
	print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
	print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
	std::cout << std::boolalpha;
	std::cout << is_container<std::vector<int>>::value << std::endl;
	std::cout << is_container<std::vector<short>>::value << std::endl;
	std::cout << is_container<std::list<short>>::value << std::endl;
	std::cout << is_container<std::list<int>>::value << std::endl;
	std::cout << are_all_same<int>::value << std::endl;
	std::cout << are_all_same<int, int>::value << std::endl;
	std::cout << are_all_same<int>::value << std::endl;
	std::cout << are_all_same<int, int, char>::value << std::endl;
	std::cout << is_uniform_tuple<std::tuple<int, int, int>>::value << std::endl;
	std::cout << is_tuple<std::tuple<int, int, int>>::value << std::endl;
	std::cout << is_uniform_tuple<std::tuple<int, int, char>>::value << std::endl;

	std::cout << is_container_v<std::vector<int>> << std::endl;
	std::cout << is_container_v<std::vector<short>> << std::endl;
	std::cout << is_container_v<std::list<short>> << std::endl;
	std::cout << is_container_v<std::list<int>> << std::endl;
	std::cout << are_all_same_v<int> << std::endl;
	std::cout << are_all_same_v<int, int> << std::endl;
	std::cout << are_all_same_v<int> << std::endl;
	std::cout << are_all_same_v<int, int, char> << std::endl;
	std::cout << is_tuple_v<std::tuple<int, int, int>> << std::endl;
	std::cout << is_uniform_tuple_v<std::tuple<int, int, int>> << std::endl;
	std::cout << is_uniform_tuple_v<std::tuple<int, int, char>> << std::endl;
	detail::print_tuple(std::make_tuple(123, 456, 789, 0));
	std::cout << std::vector{1,2,3} << std::endl;
	std::cout << std::list<short>{1,2,3} << std::endl;
	return 0;
}