#pragma once

#include<cstddef>
#include<memory>

class ArenaAllocator{
	public:
		explicit ArenaAllocator(std::size_t capacity);
		~ArenaAllocator();

		ArenaAllocator(const ArenaAllocator&) = delete;
	    ArenaAllocator& operator=(const ArenaAllocator&) = delete;

		void* allocate(
			std::size_t size,
			std::size_t alignment = alignof(std::max_align_t)
		);

		void reset() noexcept;

		std::size_t capacity() const noexcept;
		std::size_t offset() const noexcept;
	
	private:
		std::byte* buffer_;
		std::size_t capacity_;
		std::size_t offset_;
};

