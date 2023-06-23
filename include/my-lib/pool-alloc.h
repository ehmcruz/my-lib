#ifndef __MY_LIBS_POOL_ALLOC_HEADER_H__
#define __MY_LIBS_POOL_ALLOC_HEADER_H__

#include <iostream>
#include <initializer_list>
#include <vector>

#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{
namespace Alloc
{
namespace Pool
{

// ---------------------------------------------------

/*
	Sources where I based:

	https://github.com/erincatto/box2d
	https://github.com/mtrebi/memory-allocators
	Boost Pool allocator
*/

// ---------------------------------------------------

inline const size_t default_block_size = 16 * 1024; // 16KB

// ---------------------------------------------------

class Core
{
private:
	struct Chunk {
		// We can store two things in the chunks.
		// When the chunk is free, it stores the free_chunks linked link.
		// Otherwise, it stores the actual user data.
		Chunk *next_chunk;
	};

	struct Block {
		Chunk *chunks;
		
		// When we use all the allocted memory, we allocate another block.
		Block *next_block;
	};

	uint32_t chunks_per_block;
	Block *blocks;
	Chunk *free_chunks;

	OO_ENCAPSULATE_READONLY(size_t, type_size)
	OO_ENCAPSULATE_READONLY(size_t, chunk_size)

public:
	Core (size_t type_size, uint32_t chunks_per_block);
	~Core ();

	// allocates one element of size chunk_size
	inline void* alloc ()
	{
		void *free_chunk;

		if (bunlikely(this->free_chunks == nullptr))
			this->alloc_new_block();

		free_chunk = this->free_chunks;
		this->free_chunks = this->free_chunks->next_chunk;

		return free_chunk;
	}

	// free one element of size chunk_size
	void release (void *p);

	static constexpr size_t lowest_chunk_size ()
	{
		return sizeof(void*);
	}

private:
	void alloc_new_block ();
	void alloc_chunks_for_block (Block *block);
};

// ---------------------------------------------------

template <typename T>
class SameType: public Core
{
public:
	SameType (uint32_t chunks_per_block)
		: Core(sizeof(T), chunks_per_block)
	{
	}

	inline T* alloc ()
	{
		return static_cast<T*>( this->Core::alloc() );
	}

	inline void release (T *p)
	{
		this->Core::release( static_cast<void*>(p) );
	}
};

// ---------------------------------------------------

class Manager
{
private:
	// Maximum type_size handled by the allocator.
	// Any size greater than it will be directly forwarded to malloc/free.
	size_t max_size;
	
	std::vector<Core*> allocators;
	std::vector<Core*> allocators_index;

	void load (std::vector<size_t>& list_sizes, size_t max_block_size);

public:
	// max_block_size: max amount of memory to be allocated per malloc
	Manager (std::vector<size_t>& list_sizes, size_t max_block_size=default_block_size);
	Manager (std::initializer_list<size_t> list_sizes, size_t max_block_size=default_block_size);
	Manager (size_t max_size, size_t step_size, size_t max_block_size=default_block_size);

	~Manager ();

	inline void* alloc (size_t size)
	{
		void *p;

		if (blikely(size <= this->max_size))
			p = this->allocators_index[size]->alloc();
		else {
			p = malloc(size);
			assert(p != nullptr);
		}

		return p;
	}

	// it is safer to explic set the type
	// although here it doesn't even compile if you don't
	template <typename T>
	inline T* alloc ()
	{
		//std::cout << "alloc type size " << sizeof(T) << std::endl;
		return static_cast<T*>( this->alloc(sizeof(T)) );
	}

	// size here is the size of the allocated type
	inline void release (void *p, size_t size)
	{
		if (blikely(size <= this->max_size))
			this->allocators_index[size]->release(p);
		else
			free(p);
	}

	// it is safer to explic set the type
	template <typename T>
	inline void release (T *p)
	{
		//std::cout << "free type size " << sizeof(T) << std::endl;
		this->release(static_cast<void*>(p), sizeof(T));
	}
};

// ---------------------------------------------------

// still under development

#if 0
class datablock_general_alloc_t
{
public:
	using size_t = size_t;

private:
	enum class block_status_t {
		free,
		occupied
	};

	/*
		This class handle chunks differently from Core.
		In Core, the pre-allocated chunks:
			- when a block is free, it stores a linked list of free chunks;
			- when a blodk is occupied, it is 100% used (except when
			  element size < sizeof(void*)) to store the payload.
		Therefore, when a block is occupied, no memory is "wasted".
		The block and payload occupies the same address.

		However, this class (datablock_general_alloc_t) can't work like that,
		because I decided to keep a permanent double-linked list for each chunk's chunks.
		The reason is that I want to be abl to merge consecutive free chunks into one
		when a deallocation is performed, in order to reduce fragmentation.
		Therefore, each block first contains its metadata (Chunk), followed by the payload.
	*/

	struct Block;

	struct Chunk {
		Block *chunk;
		Chunk *left;
		Chunk *right;
		Chunk *previous_free_block;
		Chunk *next_free_block;
		size_t payload_capacity;
		block_status_t status;
	};

	struct Block {
		Chunk *chunks;
		Block *next_chunk;
		uint32_t chunk_size; // chunk_size can be higher than target_chunk_size to allow large allocations
	};

	Block *chunks;
	Chunk *free_chunks;

	OO_ENCAPSULATE_READONLY(size_t, target_chunk_size)

public:
	datablock_general_alloc_t (size_t target_chunk_size=(1024*128));
	~datablock_general_alloc_t ();

	void* alloc (size_t size);
	void release (void *p);

private:
	Chunk* alloc_new_chunk (size_t payload_size);
	Chunk* find_free_block (size_t payload_size);
	Chunk* split_block (Chunk *block, size_t payload_size);

	constexpr size_t calculate_required_space (size_t payload_size)
	{
		return payload_size + sizeof(Chunk);
	}

	constexpr size_t calculate_payload_size (size_t allocated_capacity)
	{
		return allocated_capacity - sizeof(Chunk);
	}
};
#endif

// ---------------------------------------------------

} // end namespace Pool
} // end namespace Alloc
} // end namespace Mylib

#endif