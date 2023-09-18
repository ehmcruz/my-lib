#include <algorithm>

#include <my-lib/memory.h>

namespace Mylib
{
namespace Memory
{

// ---------------------------------------------------

[[nodiscard]] void* StdManager::allocate (const size_t size, const std::align_val_t align)
{
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		return ::operator new(size, align);
#endif

	return ::operator new(size);
}

void StdManager::deallocate (const void *p, const size_t size, const std::align_val_t align)
{
#if __cpp_aligned_new
	if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
		::operator delete(p,
	#if __cpp_sized_deallocation
		size,
	#endif
		align));
		return;
	}
#endif
	::operator delete(p
#if __cpp_sized_deallocation
	, size
#endif
	);
}

// ---------------------------------------------------

} // end namespace Memory
} // end namespace Mylib
