#include <algorithm>

#include "datablock-alloc.h"

datablock_alloc_core_t::datablock_alloc_core_t (uint32_t type_size, uint32_t blocks_per_chunk)
{
	this->type_size = type_size;

	// we need space to store at least a pointer in each block, for the linked list of free blocks

	if (type_size < datablock_alloc_core_t::lowest_block_size())
		this->block_size = datablock_alloc_core_t::lowest_block_size();
	else
		this->block_size = type_size;

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

	chunk->blocks = static_cast<block_t*>( malloc(this->block_size * this->blocks_per_chunk) );
	assert(chunk->blocks != nullptr);

	block = chunk->blocks;

	for (uint32_t i=0; i<this->blocks_per_chunk-1; i++) {
		block->next_block = reinterpret_cast<block_t*>( reinterpret_cast<uint8_t*>(block) + this->block_size );
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

datablock_alloc_t::datablock_alloc_t (std::vector<uint32_t>& list_sizes, uint32_t max_chunk_size)
{
	this->load(list_sizes, max_chunk_size);
}

datablock_alloc_t::datablock_alloc_t (std::initializer_list<uint32_t> list_sizes, uint32_t max_chunk_size)
{
	std::vector<uint32_t> v = list_sizes;
	this->load(v, max_chunk_size);
}

datablock_alloc_t::datablock_alloc_t (uint32_t max_size, uint32_t step_size, uint32_t max_chunk_size)
{
	std::vector<uint32_t> list_sizes;

	list_sizes.reserve(max_size / step_size + 5); // 1...2...5... whatever

	for (uint32_t size=step_size; size<max_size; size+=step_size)
		list_sizes.push_back(size);
	list_sizes.push_back(max_size);
	
	this->load(list_sizes, max_chunk_size);
}

datablock_alloc_t::~datablock_alloc_t ()
{
	for (datablock_alloc_core_t *allocator: this->allocators)
		delete allocator;
}

void datablock_alloc_t::load (std::vector<uint32_t>& list_sizes, uint32_t max_chunk_size)
{
	// we remove values lower than the minimum
	std::for_each(list_sizes.begin(), list_sizes.end(),
		[] (uint32_t& v) -> void {
			if (v < datablock_alloc_core_t::lowest_block_size())
				v = datablock_alloc_core_t::lowest_block_size();
		}
	);

	// we need the allocators to be sorted in order to create the index
	std::sort(list_sizes.begin(), list_sizes.end(),
		[] (uint32_t a, uint32_t b) -> bool {
			return (a < b);
		}
	);

	// let's also remove any duplicate entries
	auto last = std::unique(list_sizes.begin(), list_sizes.end());
	list_sizes.erase(last, list_sizes.end());

	this->allocators.reserve( list_sizes.size() );

	for (uint32_t size: list_sizes) {
		uint32_t blocks_per_chunk = max_chunk_size / size;
		datablock_alloc_core_t *allocator = new datablock_alloc_core_t(size, blocks_per_chunk);
		this->allocators.push_back(allocator);
	}

	this->max_size = (*(this->allocators.end() - 1))->get_block_size();

#if 0
	for (datablock_alloc_core_t *allocator: this->allocators)
		std::cout << "alloc size " << allocator->get_block_size() << std::endl;
	std::cout << "max size is " << this->max_size << std::endl;
#endif

	// now, let's create an index for a O(1) time complexity

	this->allocators_index.resize(this->max_size + 1, nullptr);

	uint32_t size = 1;
	for (datablock_alloc_core_t *allocator: this->allocators) {
		while (size <= allocator->get_block_size()) {
			this->allocators_index[size] = allocator;
			size++;
		}
	}

#if 0
	for (uint32_t i=0; i<this->allocators_index.size(); i++) {
		uint32_t s = (this->allocators_index[i] == nullptr) ? 0 : this->allocators_index[i]->get_block_size();
		std::cout << "index " << i << " size " << s << std::endl;
	}
#endif
}
