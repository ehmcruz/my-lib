#ifndef __MY_LIBS_DATA_MACROS_HEADER_H__
#define __MY_LIBS_DATA_MACROS_HEADER_H__

#include <iostream>
#include <string>

// ---------------------------------------------------

#define DEBUG

// ---------------------------------------------------

#ifdef DEBUG
	#define dprint(STR) { std::cout << STR; }
	#define dprintln(STR) { std::cout << STR << std::endl; }
#else
	#define dprint(STR)
	#define dprintln(STR)
#endif

// ---------------------------------------------------

#define OO_ENCAPSULATE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE(TYPE, VAR) \
	OO_ENCAPSULATE_READONLY(TYPE, VAR) \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE& get_##VAR () { \
			return this->VAR; \
		} \
		inline const TYPE& get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE(TYPE, VAR) \
	OO_ENCAPSULATE_REFERENCE_READONLY(TYPE, VAR) \
	public: \
		inline void set_##VAR (const TYPE& VAR) { \
			this->VAR = VAR; \
		} \
		inline void set_##VAR (const TYPE&& VAR) { \
			this->VAR = VAR; \
		} \
	protected:

#define ASSERT(V) ASSERT_PRINT(V, "bye!\n")

#define ASSERT_PRINT(V, STR) \
	if (!(V)) [[unlikely]] { \
		std::string assert_str_ = (STR); \
		std::cout << "sanity error!" << std::endl << "file " << __FILE__ << " at line " << __LINE__ << " assertion failed!" << std::endl << #V << std::endl; \
		std::cout << assert_str_ << std::endl; \
		exit(1); \
	}

// ---------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
	#define MYLIB_ALIGN_STRUCT(V)   __attribute__((aligned(V)))
#elif defined(_MSC_VER)
	#define MYLIB_ALIGN_STRUCT(V)   __declspec(align(V))
#else
	#error "Unknown compiler. Can't define MYLIB_ALIGN_STRUCT"
#endif

#endif