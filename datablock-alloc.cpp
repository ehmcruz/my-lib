#include "datablock-alloc.h"

datablock_alloc_core_t::datablock_alloc_core_t (uint32_t type_size, uint32_t blocks_per_chunk)
{
	// we need space to store at least a pointer in each block, for the linked list of free blocks

	if (type_size < sizeof(void*))
		this->type_size = sizeof(void*);
	else
		this->type_size = type_size;

	this->blocks_per_chunk = blocks_per_chunk;

	this->free_blocks = nullptr;
	this->chunks = nullptr;
}

datablock_alloc_core_t::~datablock_alloc_core_t ()
{
	chunk_t *chunk, *next;

	for (chunk=this->chunks; chunk!=nullptr; chunk=next) {
		next = chunk->next_chunk;
		free(chunk->blocks);
		delete chunk;
	}
}

void datablock_alloc_core_t::alloc_new_chunk ()
{
	chunk_t *new_chunk;
	
	new_chunk = new chunk_t;
	this->alloc_blocks_for_chunk(new_chunk);
	new_chunk->next_chunk = this->chunks;
	this->chunks = new_chunk;
}

void datablock_alloc_core_t::alloc_blocks_for_chunk (chunk_t *chunk)
{
	block_t *block;

	// First, we allocate memory for #blocks_per_chunk elements.
	// Remember that we don't use sizeof(T) because we need memory for at least a pointer.
	// When the block is empty, it is part of the free_blocks linked_list.
	// When we remove an element, we remove its memory from the free_blocks linked list
	//   and use the memory to store user data instead.

	chunk->blocks = static_cast<block_t*>( malloc(this->type_size * this->blocks_per_chunk) );
	assert(chunk->blocks != nullptr);

	block = chunk->blocks;

	for (uint32_t i=0; i<this->blocks_per_chunk-1; i++) {
		block->next_block = reinterpret_cast<block_t*>( reinterpret_cast<uint8_t*>(block) + this->type_size );
		block = block->next_block;
	}

	/*
		now, we setup the last allocated block

		when free_blocks is empty, last block points to nothing
		when free_blocks still has blocks, we merge the lists
	*/
	
	block->next_block = this->free_blocks;
	this->free_blocks = chunk->blocks;
}

void datablock_alloc_core_t::release (void *p)
{
	block_t *block = static_cast<block_t*>(p);

	// we just add the just-freed block as the new head of the free_blocks list
	block->next_block = this->free_blocks;
	this->free_blocks = block;
}
