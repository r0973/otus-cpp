#pragma once

#include <vector>
#include <istream>
#include <cstdint>
#include <algorithm>

using ip4_part_type = std::uint8_t; 
using ip4_parts_type = std::vector<ip4_part_type>;
using ip4_pool_type = std::vector<ip4_parts_type>;

ip4_pool_type read_ip4_pool(std::istream& );
ip4_pool_type read_ip4_pool(const std::string& );
std::ostream& operator << (std::ostream& , const ip4_pool_type& );
ip4_pool_type filter(const ip4_pool_type& , const std::pair<std::size_t, ip4_part_type> & );
ip4_pool_type filter(const ip4_pool_type& , const std::pair<std::size_t, ip4_part_type> & , 
										    const std::pair<std::size_t, ip4_part_type> & );
ip4_pool_type filter(const ip4_pool_type& , ip4_part_type );

template<typename InputIt>
void reverse_sort( InputIt first, InputIt last )
{
	std::sort(first, last, []( const auto& lhs, const auto& rhs)
		{
			auto [it1, it2] = std::mismatch(std::cbegin(lhs), std::cend(lhs), std::cbegin(rhs), std::cend(rhs));
			return !(*it1 < *it2);
		});
}

std::vector<std::string> split(const std::string& str, char d);
ip4_parts_type ip4_to_parts(const std::vector<std::string>& ip4_str);
