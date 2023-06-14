#ifndef __MY_LIBS_DATA_FACTORY_HEADER_H__
#define __MY_LIBS_DATA_FACTORY_HEADER_H__

/*
	Based on Box2D block allocator.
	https://box2d.org/

	Box2D allocator is very smart, but I wanted a simpler version of it.

	Class to allocate memory in blocks, instead of doing a malloc/new per element.
	Only allocates when all free positions are used.
	When the user doesn't need the memory anymore and call free, it won't de-allocate the memory. It will put it in the free list instead;
*/

#include <iostream>
#include <initializer_list>
#include <vector>

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "macros.h"

// ---------------------------------------------------

class datablock_alloc_core_t
{
private:
	struct block_t {
		block_t *next_block;
	};

	struct chunk_t {
		// We can store two things in the blocks.
		// When the block is free, it stores the free_blocks linked link.
		// Otherwise, it stores the actual user data.
		block_t *blocks;
		
		// When we use all the allocted memory, we allocate another chunk.
		chunk_t *next_chunk;
	};

	uint32_t blocks_per_chunk;
	chunk_t *chunks;
	block_t *free_blocks;

	OO_ENCAPSULATE_READONLY(uint32_t, type_size)
	OO_ENCAPSULATE_READONLY(uint32_t, block_size)

public:
	datablock_alloc_core_t(uint32_t type_size, uint32_t blocks_per_chunk=1024);
	~datablock_alloc_core_t ();

	// allocates one element of size block_size
	inline void* alloc ()
	{
		void *free_block;

		if (bunlikely(this->free_blocks == nullptr))
			this->alloc_new_chunk();

		free_block = this->free_blocks;
		this->free_blocks = this->free_blocks->next_block;

		return free_block;
	}

	// free one element of size block_size
	void release (void *p);

	inline static uint32_t lowest_block_size ()
	{
		return sizeof(void*);
	}

private:
	void alloc_new_chunk ();	
	void alloc_blocks_for_chunk (chunk_t *chunk);
};

// ---------------------------------------------------

template <typename T>
class datablock_alloc_same_type_t: public datablock_alloc_core_t
{
public:
	datablock_alloc_same_type_t (uint32_t blocks_per_chunk=1024)
		: datablock_alloc_core_t(sizeof(T), blocks_per_chunk)
	{
	}

	inline T* alloc ()
	{
		return static_cast<T*>( this->datablock_alloc_core_t::alloc() );
	}

	inline void release (T *p)
	{
		this->datablock_alloc_core_t::release( static_cast<void*>(p) );
	}
};

// ---------------------------------------------------

class datablock_alloc_t
{
private:
	// Maximum type_size handled by the allocator.
	// Any size greater than it will be directly forwarded to malloc/free.
	uint32_t max_size;
	
	std::vector<datablock_alloc_core_t*> allocators;
	std::vector<datablock_alloc_core_t*> allocators_index;

	void load (std::vector<uint32_t>& list_sizes, uint32_t max_chunk_size);

public:
	// max_chunk_size: max amount of memory to be allocated per malloc
	datablock_alloc_t (std::vector<uint32_t>& list_sizes, uint32_t max_chunk_size=(1024*16));
	datablock_alloc_t (std::initializer_list<uint32_t> list_sizes, uint32_t max_chunk_size=(1024*16));
	datablock_alloc_t (uint32_t max_size, uint32_t step_size, uint32_t max_chunk_size=(1024*16));

	~datablock_alloc_t ();

	inline void* alloc (uint32_t size)
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

	inline void release (void *p, uint32_t size)
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

#endif