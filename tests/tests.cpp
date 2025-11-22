#include <gtest/gtest.h>
#include "../include/ChunkMemoryResource.h"
#include "Vector.h"

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

class CountingMemoryResource : public std::pmr::memory_resource {
private:
    std::pmr::memory_resource* upstream_;
    size_t alloc_count_;
    size_t dealloc_count_;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        alloc_count_++;
        return upstream_->allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        dealloc_count_++;
        upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    CountingMemoryResource(std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : upstream_(upstream), alloc_count_(0), dealloc_count_(0) {}
    
    size_t allocation_count() const { return alloc_count_; }
    size_t deallocation_count() const { return dealloc_count_; }
};

TEST(ChunkMemoryResourceTest, MemoryReuse) {
    ChunkMemoryResource resource;
    
    void* first = resource.allocate(100, 1);
    resource.deallocate(first, 100, 1);
    void* second = resource.allocate(100, 1);
    
    EXPECT_EQ(first, second);
}

TEST(ChunkMemoryResourceTest, CleanupOnDestruction) {
    CountingMemoryResource upstream;
    {
        ChunkMemoryResource resource(&upstream);
        resource.allocate(100, 1);
    }
    EXPECT_EQ(upstream.allocation_count(), upstream.deallocation_count());
}

TEST(VectorTest, IntVector) {
    Vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(VectorTest, ComplexType) {
    Vector<Point> vec;
    vec.push_back(Point(1, 2));
    vec.push_back(Point(3, 4));

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0].x, 1);
    EXPECT_EQ(vec[1].y, 4);
}

TEST(VectorTest, Iterator) {
    Vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    auto it = vec.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, vec.end());
}

TEST(VectorTest, MemoryResourceUsage) {
    ChunkMemoryResource resource;
    auto alloc = std::pmr::polymorphic_allocator<int>(&resource);
    Vector<int, std::pmr::polymorphic_allocator<int>> vec{alloc};
    
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_GE(vec.capacity(), 100);
}