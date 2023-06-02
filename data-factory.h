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

class data_factory_t
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

	uint32_t block_size;
	uint32_t blocks_per_chunk;
	chunk_t *chunks;
	block_t *free_blocks;

public:
	data_factory_t (uint32_t block_size, uint32_t blocks_per_chunk=1024);
	~data_factory_t ();

	inline void* alloc (uint32_t size)
	{
		void *free_memory;

		if (this->free_blocks == nullptr)
			this->alloc_new_chunk();

		free_memory = this->free_blocks;
		this->free_blocks = this->free_blocks->next_block;

		return free_memory;
	}

	inline void* safe_alloc (uint32_t size)
	{
		assert(size <= this->block_size);
		void *p = this->alloc(size);
		assert(p != nullptr);

		return p;
	}

private:
	void alloc_new_chunk ();	
	void alloc_blocks_for_chunk (chunk_t *chunk);
};

template <typename T>
class data_factory_same_type_t: public data_factory_t
{
public:
	data_factory_same_type_t (uint32_t blocks_per_chunk=1024)
		: data_factory_t(sizeof(T), blocks_per_chunk)
	{
	}

	using data_factory_t::alloc;

	inline T* alloc ()
	{
		void *p = this->alloc( sizeof(T) );
		return static_cast<T*>(p);
	}

	inline T* safe_alloc ()
	{
		void *p = this->alloc( sizeof(T) );
		assert(p != nullptr);
		return static_cast<T*>(p);
	}
};

#endif