#include "../include/ChunkMemoryResource.h"
#include <memory>

ChunkMemoryResource::ChunkMemoryResource(std::pmr::memory_resource* upstream)
    : allocated_blocks(upstream), 
      free_blocks(upstream), 
      upstream_resource(upstream) {}

ChunkMemoryResource::~ChunkMemoryResource() {
    for (const auto& block : allocated_blocks) {
        upstream_resource->deallocate(block.ptr, block.size, block.alignment);
    }
    for (const auto& block : free_blocks) {
        upstream_resource->deallocate(block.ptr, block.size, block.alignment);
    }
}

void* ChunkMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    auto it = free_blocks.begin();
    while (it != free_blocks.end()) {
        if (it->size >= bytes && it->alignment == alignment) {
            void* ptr = it->ptr;
            allocated_blocks.emplace_back(ptr, bytes, alignment);
            free_blocks.erase(it);
            return ptr;
        }
        ++it;
    }
    
    void* ptr = upstream_resource->allocate(bytes, alignment);
    allocated_blocks.emplace_back(ptr, bytes, alignment);
    return ptr;
}

void ChunkMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
    auto it = allocated_blocks.begin();
    while (it != allocated_blocks.end()) {
        if (it->ptr == p) {
            free_blocks.emplace_back(p, bytes, alignment);
            allocated_blocks.erase(it);
            return;
        }
        ++it;
    }
}

bool ChunkMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}