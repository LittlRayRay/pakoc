#pragma once

#include<cstddef>
#include<memory>
#include <utility>

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

		template <typename T, typename... Args>
		T* make(Args&&... args) {
			void* memory = allocate(sizeof(T), alignof(T));

			if (memory == nullptr) {
				return nullptr;
			}

			T* ptr = static_cast<T*>(memory);

			return std::construct_at(ptr, std::forward<Args>(args)...);
		}

		void reset() noexcept;

		std::size_t capacity() const noexcept;
		std::size_t offset() const noexcept;

		std::size_t used_bytes() const noexcept;

	private:
		std::byte* buffer_;
		std::size_t capacity_;
		std::size_t offset_;
		std::size_t used_bytes_;
};

