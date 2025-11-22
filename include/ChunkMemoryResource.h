#ifndef CHUNKMEMORYRESOURCE_H
#define CHUNKMEMORYRESOURCE_H

#include <memory_resource>
#include <list>
#include <cstddef>

class ChunkMemoryResource : public std::pmr::memory_resource {
private:
    struct Block {
        void* ptr;
        std::size_t size;
        std::size_t alignment;
        
        Block(void* p, std::size_t s, std::size_t a) 
            : ptr(p), size(s), alignment(a) {}
    };
    
    std::pmr::list<Block> allocated_blocks;
    std::pmr::list<Block> free_blocks;
    std::pmr::memory_resource* upstream_resource;

public:
    explicit ChunkMemoryResource(std::pmr::memory_resource* upstream = std::pmr::get_default_resource());
    ~ChunkMemoryResource();

    ChunkMemoryResource(const ChunkMemoryResource&) = delete;
    ChunkMemoryResource& operator=(const ChunkMemoryResource&) = delete;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};

#endif