#pragma once

#include <memory>
#include <iostream>


template <typename T, std::size_t PoolSize>
class MemoryAllocator
{
public:
    using value_type = T;

    MemoryAllocator()
	: pool(new char[PoolSize * sizeof(T)])
	, available_memory(PoolSize * sizeof(T))
	{
        #ifndef NDEBUG
            monitor("Memory pool allocated with size: ", PoolSize);
        #endif
    }

    ~MemoryAllocator()
	{
        delete[] pool;
        
		#ifndef NDEBUG
            std::cout << "Memory pool deallocated" << std::endl;;
        #endif
    }

    template <typename U>
    MemoryAllocator(const MemoryAllocator<U, PoolSize>&)
    : pool(nullptr)
    , available_memory(0)
    {}

    T* allocate(std::size_t n)
	{
        if (n * sizeof(T) > available_memory)
            throw std::bad_alloc();

        T* result = reinterpret_cast<T*>(pool + (PoolSize * sizeof(T) - available_memory));
        available_memory -= n * sizeof(T);
        
		#ifndef NDEBUG
            monitor("Allocated ", n);
        #endif
		
		return result;
    }

    void deallocate([[maybe_unused]]T* ptr, [[maybe_unused]]std::size_t n)
    {
        #ifndef NDEBUG
            monitor("Deallocated ", n);
        #endif
    }

	template<class U>
    struct rebind
    {
        using other = MemoryAllocator<U, PoolSize>;
    };

private:
	void monitor(const std::string& msg, std::size_t n)
	{
		std::cout << msg << " " 
                  << n << " objects. Available memory remains: " 
                  << available_memory << " bytes" 
                  << std::endl;
	}

private:
    char* pool;
    std::size_t available_memory;
};

template <typename T, typename U, std::size_t PoolSize1, std::size_t PoolSize2>
bool operator==(const MemoryAllocator<T, PoolSize1>& lhs, const MemoryAllocator<U, PoolSize2>& rhs)
{
	return (lhs.pool == rhs.pool) && (PoolSize1 == PoolSize2);
}

template <typename T, typename U, std::size_t PoolSize1, std::size_t PoolSize2>
bool operator!=(const MemoryAllocator<T, PoolSize1>& lhs, const MemoryAllocator<U, PoolSize2>& rhs)
{
    return (lhs.pool != rhs.pool) || (PoolSize1 != PoolSize2);
}