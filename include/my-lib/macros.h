#ifndef __MY_LIBS_DATA_MACROS_HEADER_H__
#define __MY_LIBS_DATA_MACROS_HEADER_H__

#include <iostream>
#include <string>

// ---------------------------------------------------

#define DEBUG

// ---------------------------------------------------

#ifdef DEBUG
	#define dprint(STR) { std::cout << STR; }
#else
	#define dprint(STR)
#endif

// ---------------------------------------------------

#define OO_ENCAPSULATE(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE& VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE& get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE& get_##VAR () const { \
			return this->VAR; \
		} \
	protected:

#define ASSERT(V) ASSERT_PRINT(V, "bye!\n")

#define ASSERT_PRINT(V, STR) \
	if (bunlikely(!(V))) { \
		std::string assert_str_ = (STR); \
		std::cout << "sanity error!" << std::endl << "file " << __FILE__ << " at line " << __LINE__ << " assertion failed!" << std::endl << #V << std::endl; \
		std::cout << assert_str_ << std::endl; \
		exit(1); \
	}

#endif