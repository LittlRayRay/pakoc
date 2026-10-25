#include "allocators/arena_allocator.h"

#include <new>
#include <stdexcept>
#include <memory>
#include <utility>
#include <cstddef>

ArenaAllocator::ArenaAllocator(std::size_t capacity) 
    : buffer_(nullptr), 
      capacity_(capacity), 
      offset_(0),
      used_bytes_(0) {
    if (capacity == 0) {
        throw std::invalid_argument(
            "Arena capacity must be greater than zero"
        );
    }

    buffer_ = static_cast<std::byte*>(
        ::operator new(
            capacity_,
            std::align_val_t{alignof(std::max_align_t)}
        )
    );
}

ArenaAllocator::~ArenaAllocator()
{
    ::operator delete(
        buffer_,
        std::align_val_t{alignof(std::max_align_t)}
    );
}

void* ArenaAllocator::allocate(
    std::size_t size,
    std::size_t alignment
) {
    if (size == 0) {
        return nullptr;
    }

    // check if alignment is non-zero and a power of 2 
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        return nullptr;
    }

    void* ptr = buffer_ + offset_;

    // space left in the buffer
    std::size_t remaining = capacity_ - offset_;

    void* aligned_ptr = std::align(
        alignment,
        size,
        ptr,
        remaining
    );

    if (aligned_ptr == nullptr) return nullptr;

    auto* result = static_cast<std::byte*>(aligned_ptr);
    
    offset_ = static_cast<std::size_t> (
        result - buffer_
    ) + size;

    used_bytes_ += size;
    return result;
}

void ArenaAllocator::reset() {
    offset_ = 0;
    used_bytes_ = 0;
}

std::size_t ArenaAllocator::capacity() const noexcept {
    return capacity_;
}

std::size_t ArenaAllocator::offset() const noexcept {
    return offset_;
}

std::size_t ArenaAllocator::used_bytes() const noexcept {
    return used_bytes_;
}