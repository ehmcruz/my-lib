#ifndef __MY_LIBS_STD_HEADER_H__
#define __MY_LIBS_STD_HEADER_H__

#include <cstdint>

namespace Mylib
{

// ---------------------------------------------------

using size_t = std::size_t;

// ---------------------------------------------------

inline void* alloc (size_t size, std::align_val_t align)
{
	return ::operator new(size, align);
}

inline void free (void *p)
{
	::operator delete(p);
}

// ---------------------------------------------------

} // end namespace Mylib

#endif