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

template <int block_size>
class data_factory_t
{
private:
	struct block_t {
		block *next_block;
	};

	struct chunk_t {
		// We can store two things in the blocks.
		// When the block is free, it stores the free_blocks linked link.
		// Otherwise, it stores the actual user data.
		block_t *blocks;
		
		// When we use all the allocted memory, we allocate another chunk.
		chunk_t *next_chunk;
	};

	//uint32_t block_size;
	uint32_t blocks_per_chunk;
	chunk_t *chunks, *last_chunk;
	block_t *free_blocks;

public:
	object_factory_t (uint32_t blocks_per_chunk)
	{
		// we need space to store at least a pointer in each block, for the linked list of free blocks
		static_assert(block_size >= sizeof(void*));

		this->blocks_per_chunk = blocks_per_chunk;

		// this needs to be set before calling alloc_blocks_for_chunk
		// otherwise, panic
		this->free_blocks = nullptr;
		
		this->chunks = new chunk_t;
		this->alloc_blocks_for_chunk( this->chunks );
		this->chunks->next_chunk = nullptr;

		this->last_chunk = this->last_chunk;
	}

	~object_factory_t ()
	{
		chunk_t *chunk, *next;

		for (chunk=this->chunks; chunk!=nullptr; chunk=next) {
			next = chunk->next_chunk;
			free(chunk->blocks);
			delete chunk;
		}
	}

	void* alloc (uint32_t size)
	{
		void *free_memory;

		assert(size <= block_size);

		if (this->free_blocks == nullptr) {
			this->last_chunk->next_chunk = new chunk_t;
			this->last_chunk = this->last_chunk->next_chunk;
			this->alloc_blocks_for_chunk( this->last_chunk );
			this->last_chunk->next_chunk = nullptr;
		}

		free_memory = this->free_blocks;
		this->free_blocks = this->free_blocks->next_block;

		return free_memory;
	}

private:

	void alloc_blocks_for_chunk (chunk_t *chunk)
	{
		block_t *block;

		// First, we allocate memory for #blocks_per_chunk elements.
		// Remember that we don't use sizeof(T) because we need memory for at least a pointer.
		// When the block is empty, it is part of the free_blocks linked_list.
		// When we remove an element, we remove its memory from the free_blocks linked list
		//   and use the memory to store user data instead.

		chunk->blocks = malloc(this->block_size * this->blocks_per_chunk);

		if (chunk->blocks == nullptr) {
			std::cout << "error at alloc_blocks_for_chunk" << std::endl;
			exit(1);
		}

		block = chunk->blocks;

		for (uint32_t i=0; i<this->blocks_per_chunk-1; i++) {
			block->next = static_cast<block_t*>( static_cast<uint8_t*>(block) + this->block_size );
			block = block->next;
		}

		/*
			now, we setup the last allocated block

			when free_blocks is empty, last block points to nothing
			if free_blocks still has blocks, we merge the lists
		*/
		
		block->next = this->free_blocks;
		this->free_blocks = chunk->blocks;
	}
};

template <typename T>
class data_factory_same_type_t: data_factory_t< sizeof(T) >
{
public:
	T* alloc ()
	{
		return this->alloc( sizeof(T) );
	}
};

#endif