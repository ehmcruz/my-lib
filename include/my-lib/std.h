#ifndef __MY_LIBS_STD_HEADER_H__
#define __MY_LIBS_STD_HEADER_H__

#include <concepts>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

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

class Exception : public std::exception
{
private:
	std::string msg;

public:
	Exception (const std::string& msg_)
	: msg(msg_)
	{
	}

	Exception (std::string&& msg_)
	: msg(msg_)
	{
	}

	Exception (const char *msg_)
	: msg(msg_)
	{
	}

	const char* what() const noexcept override
	{
		return this->msg.data();
	}
};

#define mylib_assert_exception_diecode_msg(bool_expr, die_code, msg) \
	if (!(bool_expr)) [[unlikely]] { \
		die_code \
		std::ostringstream str_stream; \
		str_stream << "sanity error!" << std::endl << "file " << __FILE__ << " at line " << __LINE__ << " assertion failed!" << std::endl << #bool_expr << std::endl; \
		str_stream << msg << std::endl; \
		const std::string str = str_stream.str(); \
		\
		throw Mylib::Exception(str); \
	}

#define mylib_assert_exception_msg(bool_expr, msg) mylib_assert_exception_diecode_msg(bool_expr, , msg)

#define mylib_assert_exception(bool_expr) mylib_assert_exception_msg(bool_expr, "")

// ---------------------------------------------------

} // end namespace Mylib

#endif