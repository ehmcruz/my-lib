#ifndef __MY_LIB_DATA_MACROS_HEADER_H__
#define __MY_LIB_DATA_MACROS_HEADER_H__

#include <iostream>
#include <string>
#include <type_traits>

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_SCALAR(TYPE, VAR) \
	OO_ENCAPSULATE_SCALAR_READONLY(TYPE, VAR) \
	public: \
		inline void set_##VAR (const TYPE VAR) { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_CONST_READONLY(TYPE, VAR) \
	protected: \
		const TYPE VAR; \
	public: \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_INIT_READONLY(TYPE, VAR, DATA) \
	protected: \
		TYPE VAR = (DATA); \
	public: \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_SCALAR_INIT(TYPE, VAR, DATA) \
	OO_ENCAPSULATE_SCALAR_INIT_READONLY(TYPE, VAR, DATA) \
	public: \
		inline void set_##VAR (const TYPE VAR) { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_SCALAR_INIT_CONST_READONLY(TYPE, VAR, DATA) \
	protected: \
		const TYPE VAR = (DATA); \
	public: \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_PTR_READONLY(TYPE, VAR, DATA) \
	protected: \
		TYPE VAR = DATA; \
	public: \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_PTR(TYPE, VAR) \
	OO_ENCAPSULATE_PTR_READONLY(TYPE, VAR) \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#define OO_ENCAPSULATE_PTR_INIT_READONLY(TYPE, VAR, DATA) \
	protected: \
		TYPE VAR = DATA; \
	public: \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_PTR_INIT(TYPE, VAR, DATA) \
	OO_ENCAPSULATE_PTR_INIT_READONLY(TYPE, VAR, DATA) \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
	protected:

// ---------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
	#define MYLIB_ALIGN_STRUCT(V)   __attribute__((aligned(V)))
#elif defined(_MSC_VER)
	#define MYLIB_ALIGN_STRUCT(V)   __declspec(align(V))
#else
	#error "Unknown compiler. Can't define MYLIB_ALIGN_STRUCT"
#endif

#endif