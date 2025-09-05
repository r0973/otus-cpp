#pragma once

#include <memory>

// simple forward list
template <typename T, typename Allocator = std::allocator<T>>
class CustomContainer
{
private:
    struct Node
    {
        T data;
        Node* next;
    };

    using AllocTraits = std::allocator_traits<Allocator>;
    using NodeAllocator = typename AllocTraits::rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAllocator>;

    Node* head = nullptr;
    Node* tail = nullptr;
    std::size_t listSize = 0;
    NodeAllocator allocator;

public:
    using value_type = T;

    CustomContainer()
    : head{nullptr}
    , tail{nullptr}
    , listSize{0}
    , allocator{NodeAllocator{}}
    {}

    void push_back(const T& value) {
        Node* new_node = NodeAllocTraits::allocate(allocator, 1);
        NodeAllocTraits::construct(allocator, new_node, Node{value, nullptr});

        if (!head) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
        ++listSize;
    }

    void pop_front() {
        if (!head) return;
        Node* old_head = head;
        head = head->next;
        if (!head) {
            tail = nullptr;
        }
        NodeAllocTraits::destroy(allocator, old_head);
        NodeAllocTraits::deallocate(allocator, old_head, 1);
        --listSize;
    }

    ~CustomContainer()
    {
        while (head)
        {
            pop_front();
        }
    }

    // Простой итератор для обхода контейнера
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Node;
        using difference_type = std::ptrdiff_t;
        using pointer = Node*;
        using reference = T&;

    public:
        Iterator(pointer ptr) : ptr_(ptr) {}

        reference operator*() { return ptr_->data; }
        Iterator& operator++() { ptr_ = ptr_->next; return *this; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }

    private:
        pointer ptr_;
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    std::size_t size() const { return listSize; }

    bool empty() const { return listSize == 0; }
};