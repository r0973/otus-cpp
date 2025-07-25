#pragma once

#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class CustomContainer
{
public:
    using value_type = T;
    using allocator_type = Allocator;
    using Alloc = typename Allocator::template rebind<T>::other;

private:
    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    Alloc allocator_;

public:
    CustomContainer()
	: allocator_(Allocator())
	{}

    void push_back(const T& value)
	{
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        allocator_.construct(data_ + size_, value);
        ++size_;
    }

    void reserve(std::size_t new_capacity)
	{
        if (new_capacity > capacity_)
		{
            T* new_data_ = allocator_.allocate(new_capacity);
            
			for (std::size_t i = 0; i < size_; ++i)
			{
                allocator_.construct(new_data_ + i, std::move(data_[i]));
                allocator_.destroy(data_ + i);
            }
            
			if (data_)
			{
                allocator_.deallocate(data_, capacity_);
            }
            
			data_ = new_data_;
            capacity_ = new_capacity;
        }
    }

    ~CustomContainer()
	{
        for (std::size_t i = 0; i < size_; ++i)
		{
            allocator_.destroy(data_ + i);
        }
        if (data_)
		{
            allocator_.deallocate(data_, capacity_);
        }
    }

    // Iterator for container
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(pointer ptr)
        : ptr(ptr)
        {}

        reference operator*() const { return *ptr; }
        pointer operator->() { return ptr; }
        Iterator& operator++() { ++ptr; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++ptr; return tmp; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }

    private:
        pointer ptr;
    };

    Iterator begin() { return Iterator(data_); }
    Iterator end() { return Iterator(data_ + size_); }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; } 

};