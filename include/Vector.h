#ifndef VECTOR_H
#define VECTOR_H

#include <memory_resource>
#include <iterator>
#include <memory>

template<typename T, typename Allocator = std::pmr::polymorphic_allocator<T>>
class Vector {
public:
    using allocator_type = Allocator;
    
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(pointer ptr) : current(ptr) {}
        
        reference operator*() const { return *current; }
        pointer operator->() const { return current; }
        
        Iterator& operator++() {
            ++current;
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++current;
            return tmp;
        }
        
        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }

    private:
        pointer current;
    };

    Vector(const Allocator& alloc = {})
        : allocator(alloc), data(nullptr), size_(0), capacity_(0) {}

    ~Vector() {
        clear();
        if (data) {
            allocator.deallocate(data, capacity_);
        }
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        std::allocator_traits<allocator_type>::construct(allocator, data + size_, value);
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            std::allocator_traits<allocator_type>::destroy(allocator, data + size_);
        }
    }

    void reserve(std::size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        
        T* new_data = allocator.allocate(new_capacity);
        for (std::size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::construct(allocator, new_data + i, std::move(data[i]));
            std::allocator_traits<allocator_type>::destroy(allocator, data + i);
        }
        
        if (data) {
            allocator.deallocate(data, capacity_);
        }
        data = new_data;
        capacity_ = new_capacity;
    }

    void clear() {
        for (std::size_t i = 0; i < size_; ++i) {
            std::allocator_traits<allocator_type>::destroy(allocator, data + i);
        }
        size_ = 0;
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    T& operator[](std::size_t index) { return data[index]; }
    const T& operator[](std::size_t index) const { return data[index]; }

    Iterator begin() { return Iterator(data); }
    Iterator end() { return Iterator(data + size_); }

private:
    allocator_type allocator;
    T* data;
    std::size_t size_;
    std::size_t capacity_;
};

#endif