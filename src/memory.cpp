#include <algorithm>

#include <my-lib/memory.h>

namespace Mylib
{
namespace Memory
{

// ---------------------------------------------------

[[nodiscard]] void* DefaultManager::allocate (const size_t type_size, const size_t count, const uint32_t align)
{
	return m_allocate(type_size * count, align);
}

void DefaultManager::deallocate (void *p, const size_t type_size, const size_t count, const uint32_t align)
{
	m_deallocate(p, type_size * count, align);
}

// ---------------------------------------------------

} // end namespace Memory
} // end namespace Mylib
