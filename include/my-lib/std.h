#ifndef __MY_LIBS_STD_HEADER_H__
#define __MY_LIBS_STD_HEADER_H__

#include <concepts>
#include <type_traits>

#include <cstdint>

#if !(defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER))
	#warning Untested compiler
#endif

namespace Mylib
{

// ---------------------------------------------------

template<typename T>
using remove_type_qualifiers = typename std::remove_cvref<T>;
//using remove_type_qualifiers = typename std::remove_cv<typename std::remove_reference<T>::type>;

// no idea why the following causes issues
//template<typename T>
//using remove_type_qualifierss = typename remove_type_qualifiers<T>::type;

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