
#include <fstream>
#include <iostream>
#include <string>

#include "lib.h"

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
// split string by symbol
std::vector<std::string> split(const std::string& str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
} 

// split ap4 address string to parts and convert to numbers
ip4_parts_type ip4_to_parts(const std::vector<std::string>& ip4_str)
{
	ip4_parts_type ip4_parts;
	ip4_parts.reserve(4);
	for(const auto& str : ip4_str)
	{
		const int number = std::stoi(str);
		if( number < 0 || number > 255) 
		{
			throw std::out_of_range("wrong number");
		}
		ip4_parts.emplace_back(static_cast<ip4_part_type>(number)); 
	}
	return ip4_parts;
}

// read ip4 from stream
ip4_pool_type read_ip4_pool(std::istream& is)
{
	ip4_pool_type ip4_pool;
	for(std::string line; std::getline(is, line);)
	{
		const auto ip4_str = split(line, '\t');
		const auto ip4_parts_str = split(ip4_str.at(0), '.');
		if(ip4_parts_str.size() != 4)
		{
			throw std::runtime_error("wrong ip4 address");
		}
		ip4_pool.emplace_back(ip4_to_parts(ip4_parts_str));
	}
	return ip4_pool;
}

// read ip4 pool from file 
ip4_pool_type read_ip4_pool(const std::string& filename)
{
	std::ifstream ifs(filename);
	if(ifs.is_open())
	{	
		return read_ip4_pool(ifs);
	}
	else {
		std::cout << "Unable to open file" << std::endl;
	}
	return ip4_pool_type{};
}

// output ip4 address
std::ostream& operator << (std::ostream & os, const ip4_parts_type& ip4_parts)
{
	for(auto it_part = std::cbegin(ip4_parts); it_part != std::cend(ip4_parts); ++it_part)
	{
		if (it_part != std::cbegin(ip4_parts))
		{
			os << ".";
		}
		os << static_cast<int>(*it_part);
	}
	os << "\n";
    return os;
}

// output of ip4 pool
std::ostream& operator << (std::ostream & os, const ip4_pool_type& ip4_pool)
{
	for(const auto& ip4_parts : ip4_pool)
	{
		os << ip4_parts;
	}
    return os;
}

// filter values by first byte
ip4_pool_type filter(const ip4_pool_type& ip4_pool, const std::pair<std::size_t, ip4_part_type>& value)
{
	ip4_pool_type result;
	std::copy_if(std::cbegin(ip4_pool), std::cend(ip4_pool), std::back_inserter(result),
		[value](const auto& ip4_part)
		{
			return ip4_part.at(value.first) == value.second;
		}
	);
	return result; 
}

// filter values by first and second byte
ip4_pool_type filter(const ip4_pool_type& ip4_pool, const std::pair<std::size_t, ip4_part_type>& value1,
	const std::pair<std::size_t, ip4_part_type> & value2)
{
	ip4_pool_type result;
	std::copy_if(std::cbegin(ip4_pool), std::cend(ip4_pool), std::back_inserter(result),
		[value1, value2](const auto& ip4_part)
		{
			return ip4_part.at(value1.first) == value1.second && ip4_part.at(value2.first) == value2.second;
		}
	);
	return result; 
}

// filter values by any byte
ip4_pool_type filter(const ip4_pool_type& ip4_pool, ip4_part_type value)
{
	ip4_pool_type result;
	std::copy_if(std::cbegin(ip4_pool), std::cend(ip4_pool), std::back_inserter(result),
		[value](const auto& ip4_parts)
		{
			return std::any_of(std::cbegin(ip4_parts), std::cend(ip4_parts),
				[value](const auto& ip4_part) { return ip4_part == value; });
		}
	);
	return result; 
}