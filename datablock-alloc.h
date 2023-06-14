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

	uint32_t type_size;
	uint32_t blocks_per_chunk;
	chunk_t *chunks;
	block_t *free_blocks;

public:
	datablock_alloc_core_t(uint32_t type_size, uint32_t blocks_per_chunk=1024);
	~datablock_alloc_core_t ();

	// allocates one element of size type_size
	inline void* alloc ()
	{
		void *free_block;

		if (bunlikely(this->free_blocks == nullptr))
			this->alloc_new_chunk();

		free_block = this->free_blocks;
		this->free_blocks = this->free_blocks->next_block;

		return free_block;
	}

	// free one element of size type_size
	void release (void *p);

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

template <int max_size=4096, int step_size=8, int size_min_blocks=1024, int size_max_blocks=16>
class datablock_alloc_t
{
private:
	datablock_alloc_core_t *allocators[max_size+1];

public:
	datablock_alloc_t ()
	{
		uint32_t blocks_per_chunk;

		// allocator for zero bytes does not make sense
		this->allocators[0] = nullptr;

		blocks_per_chunk = 1024;

		for (uint32_t size=step_size; size<=max_size; size+=step_size) {
			this->allocators[size] = new datablock_alloc_core_t(size, blocks_per_chunk);

			for (uint32_t sub_size=size-(step_size-1); sub_size<size; sub_size++)
				this->allocators[sub_size] = this->allocators[size];
		}
	}

	~datablock_alloc_t ()
	{
		for (uint32_t size=step_size; size<=max_size; size+=step_size)
			delete this->allocators[size];
	}

	void* alloc (uint32_t size)
	{
		void *p;

		if (blikely(size <= max_size))
			p = this->allocators[size]->alloc();
		else {
			p = malloc(size);
			assert(p != nullptr);
		}

		return p;
	}

	// it is safer to explic set the type
	// although here it doesn't even compile if you don't
	template <typename T>
	T* alloc ()
	{
		//std::cout << "alloc type size " << sizeof(T) << std::endl;
		return static_cast<T*>( this->alloc(sizeof(T)) );
	}

	void release (void *p, uint32_t size)
	{
		if (blikely(size <= max_size))
			this->allocators[size]->release(p);
		else
			free(p);
	}

	// it is safer to explic set the type
	template <typename T>
	void release (T *p)
	{
		//std::cout << "free type size " << sizeof(T) << std::endl;
		this->release(static_cast<void*>(p), sizeof(T));
	}
};

#endif