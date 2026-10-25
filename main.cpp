#include "allocators/arena_allocator.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

struct alignas(64) CacheAligned {
    int value;
};

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
};

struct TestObject {
    std::string name;
    int value;

    TestObject(std::string name, int value)
        : name(std::move(name)), value(value) {}
};

struct alignas(64) AlignedObject {
    int value;

    explicit AlignedObject(int value) : value(value) {}
};

int main() {
    ArenaAllocator arena(1024);

    // Test 1: Basic allocation.
    void* a = arena.allocate(100);
    void* b = arena.allocate(200);

    assert(a != nullptr);
    assert(b != nullptr);
    assert(a != b);

    std::cout << "Used: " << arena.offset() << " bytes\n";

    // Test 2: Alignment.
    void* p = arena.allocate(
        sizeof(CacheAligned),
        alignof(CacheAligned)
    );

    assert(p != nullptr);

    assert(
        reinterpret_cast<std::uintptr_t>(p)
        % alignof(CacheAligned) == 0
    );

    // Test 3: Exhaustion.
    ArenaAllocator small(64);

    assert(small.allocate(64) != nullptr);
    assert(small.allocate(1) == nullptr);

    // Test 4: Reset and reuse.
    arena.reset();

    assert(arena.offset() == 0);
    assert(arena.allocate(1024) != nullptr);

    arena.reset();

    assert(arena.allocate(1024) != nullptr);

    std::cout << "Basic tests passed\n";
	// Test 1: Basic construction
	{
		ArenaAllocator arena(1024);

		Point* p = arena.make<Point>(10, 20);

		assert(p != nullptr);
		assert(p->x == 10);
		assert(p->y == 20);

		std::cout << "Test 1 passed: basic construction\n";
	}

	// Test 2: Forwarding multiple constructor arguments
	{
		ArenaAllocator arena(1024);

		TestObject* obj = arena.make<TestObject>("hello", 42);

		assert(obj != nullptr);
		assert(obj->name == "hello");
		assert(obj->value == 42);

		std::cout << "Test 2 passed: constructor arguments\n";

		// TestObject owns a std::string, so explicitly destroy it
		// before the arena releases its storage.
		std::destroy_at(obj);
	}

	// Test 3: Correct alignment
	{
		ArenaAllocator arena(1024);

		AlignedObject* obj = arena.make<AlignedObject>(123);

		assert(obj != nullptr);
		assert(obj->value == 123);
		assert(reinterpret_cast<std::uintptr_t>(obj) % alignof(AlignedObject) == 0);

		std::cout << "Test 3 passed: alignment\n";

		std::destroy_at(obj);
	}

	// Test 4: Multiple objects in the same arena
	{
		ArenaAllocator arena(1024);

		Point* a = arena.make<Point>(1, 2);
		Point* b = arena.make<Point>(3, 4);

		assert(a != nullptr);
		assert(b != nullptr);

		assert(a->x == 1 && a->y == 2);
		assert(b->x == 3 && b->y == 4);

		assert(a != b);

		std::cout << "Test 4 passed: multiple allocations\n";
	}

	// Test 5: Arena exhaustion
	{
		ArenaAllocator arena(64);

		// Request more memory than the arena can provide.
		void* memory = arena.allocate(1024);

		assert(memory == nullptr);

		std::cout << "Test 5 passed: allocation failure\n";
	}

	// Test 6: Reset and reuse
	{
		ArenaAllocator arena(128);

		Point* first = arena.make<Point>(10, 20);
		assert(first != nullptr);

		arena.reset();

		Point* second = arena.make<Point>(30, 40);
		assert(second != nullptr);
		assert(second->x == 30);
		assert(second->y == 40);

		std::cout << "Test 6 passed: reset and reuse\n";
	}
}