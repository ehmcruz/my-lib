#ifndef __MY_LIB_EXCEPTION_HEADER_H__
#define __MY_LIB_EXCEPTION_HEADER_H__

#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <source_location>

#include <my-lib/std.h>


namespace Mylib
{

// ---------------------------------------------------

class Exception : public std::exception
{
private:
	mutable std::string msg;
	std::source_location location;
	const char *assert_str;

public:
	Exception (const std::source_location& location_, const char *assert_str_)
		: location(location_), assert_str(assert_str_)
	{
	}

	const char* what () const noexcept override final
	{
		try {
			std::ostringstream str_stream;

			str_stream << "My-lib Exception: "
				<< "File: " << this->location.file_name() << std::endl
				<< "Line: " << this->location.line() << std::endl
				<< "Function: " << this->location.function_name() << std::endl;

			if (this->assert_str != nullptr)
				str_stream << "Assertion that failed: " << this->assert_str << std::endl;
	
			this->build_exception_msg(str_stream);
			this->msg = str_stream.str();
	
			return this->msg.c_str();
		} catch (...) {
			return "My-lib Exception: error while building exception message";
		}
	}

protected:
	virtual void build_exception_msg (std::ostringstream& str_stream) const = 0;
};

// ---------------------------------------------------

class AssertException : public Exception
{
public:
	AssertException (const std::source_location& location_, const char *assert_str_)
		: Exception(location_, assert_str_)
	{
	}

protected:
	void build_exception_msg (std::ostringstream& str_stream) const override
	{
		str_stream << "Assert exception.";
	}
};

// ---------------------------------------------------

class ZeroNumberException : public Exception
{
public:
	ZeroNumberException (const std::source_location& location_, const char *assert_str_)
		: Exception(location_, assert_str_)
	{
	}

protected:
	void build_exception_msg (std::ostringstream& str_stream) const override
	{
		str_stream << "Zero number exception.";
	}
};

// ---------------------------------------------------

/*
// This is a helper function to throw an exception with no arguments.

template <typename Texception>
void throw_exception (const std::source_location& location = std::source_location::current())
{
	throw Texception(location);
}

template <typename Texception>
void assert_exception (const bool bool_expr, const std::source_location& location = std::source_location::current())
{
	if (!bool_expr) [[unlikely]]
		throw_exception<Texception>(location);
}

// ---------------------------------------------------

// This is a helper function to throw an exception with 1 argument.

template <typename Texception, typename Ta>
void throw_exception (Ta&& arg_a, const std::source_location& location = std::source_location::current())
{
	throw Texception(location, std::forward<Ta>(arg_a));
}

template <typename Texception, typename Ta>
void assert_exception (const bool bool_expr, Ta&& arg_a, const std::source_location& location = std::source_location::current())
{
	if (!bool_expr) [[unlikely]]
		throw_exception<Texception>(location, std::forward<Ta>(arg_a));
}

// ---------------------------------------------------

// This is a helper function to throw an exception with 2 arguments.

template <typename Texception, typename Ta, typename Tb>
void throw_exception (Ta&& arg_a, Tb&& arg_b, const std::source_location& location = std::source_location::current())
{
	throw Texception(location, std::forward<Ta>(arg_a), std::forward<Tb>(arg_b));
}

template <typename Texception, typename Ta, typename Tb>
void assert_exception (const bool bool_expr, Ta&& arg_a, Tb&& arg_b, const std::source_location& location = std::source_location::current())
{
	if (!bool_expr) [[unlikely]]
		throw_exception<Texception>(location, std::forward<Ta>(arg_a), std::forward<Tb>(arg_b));
}

// This is a helper function to throw an exception with 3 arguments.

template <typename Texception, typename Ta, typename Tb, typename Tc>
void throw_exception (Ta&& arg_a, Tb&& arg_b, Tc&& arg_c, const std::source_location& location = std::source_location::current())
{
	throw Texception(location, std::forward<Ta>(arg_a), std::forward<Tb>(arg_b), std::forward<Tc>(arg_c));
}

template <typename Texception, typename Ta, typename Tb, typename Tc>
void assert_exception (const bool bool_expr, Ta&& arg_a, Tb&& arg_b, Tc&& arg_c, const std::source_location& location = std::source_location::current())
{
	if (!bool_expr) [[unlikely]]
		throw_exception<Texception>(location, std::forward<Ta>(arg_a), std::forward<Tb>(arg_b), std::forward<Tc>(arg_c));
}
*/
// ---------------------------------------------------

template <typename Texception, typename... Targs>
void throw_exception__ (const std::source_location& location, const char *assert_str, Targs&&... args)
{
	throw Texception(location, assert_str, std::forward<Targs>(args)...);
}

/*template <typename Texception, typename... Targs>
void assert_exception (const bool bool_expr, const std::source_location& location = std::source_location::current(), Targs&&... args)
{
	if (!bool_expr) [[unlikely]]
		throw_exception<Texception>(location, std::forward<Targs>(args)...);
}*/

// ---------------------------------------------------

#define mylib_throw(TYPE) \
	Mylib::throw_exception__<TYPE>(std::source_location::current(), nullptr)

#define mylib_throw_args(TYPE, ...) \
	Mylib::throw_exception__<TYPE>(std::source_location::current(), nullptr, __VA_ARGS__)

#define mylib_assert_excetion(bool_expr, TYPE) { \
	if (!(bool_expr)) [[unlikely]] \
		Mylib::throw_exception__<TYPE>(std::source_location::current(), #bool_expr); \
	}

#define mylib_assert(bool_expr) mylib_assert_excetion((bool_expr), Mylib::AssertException)

#define mylib_assert_exception_args(bool_expr, TYPE, ...) { \
	if (!(bool_expr)) [[unlikely]] \
		Mylib::throw_exception__<TYPE>(std::source_location::current(), #bool_expr, __VA_ARGS__); \
	}

	// ---------------------------------------------------

} // end namespace Mylib

#endif