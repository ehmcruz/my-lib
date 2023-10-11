#ifndef __MY_LIB_BIT_HEADER_H__
#define __MY_LIB_BIT_HEADER_H__

#include <cstdint>

namespace Mylib
{

// ---------------------------------------------------

template <typename T>
constexpr T extract_bits (const T v, const std::size_t bstart, const std::size_t blength)
{
	const T mask = (1 << blength) - 1;

	return (v >> bstart) & mask;
}

template <typename T>
constexpr T set_bits (const T v, const std::size_t bstart, const std::size_t blength, const T bits)
{
	const T mask = ((1 << blength) - 1) << bstart;

	return (v & ~mask) | (bits << bstart);
}

// ---------------------------------------------------

template <uint32_t storage_nbits>
class BitsetStorage__;

template<>
class BitsetStorage__<8>
{
public:
	using Type = uint8_t;
protected:
	Type storage;
};

template<>
class BitsetStorage__<16>
{
public:
	using Type = uint16_t;
protected:
	Type storage;
};

template<>
class BitsetStorage__<32>
{
public:
	using Type = uint32_t;
protected:
	Type storage;
};

template<>
class BitsetStorage__<64>
{
public:
	using Type = uint64_t;
protected:
	Type storage;
};

consteval uint32_t calc_fast_bit_set_storage_nbits__ (const uint32_t nbits)
{
	if (nbits <= 8)
		return 8;
	else if (nbits <= 16)
		return 16;
	else if (nbits <= 32)
		return 32;
	else if (nbits <= 64)
		return 64;
	else
		return 0;
}

template <uint32_t nbits,
          uint32_t storage_nbits = calc_fast_bit_set_storage_nbits__(nbits),
		  typename ParentType = BitsetStorage__<storage_nbits>
		  >
class Bitset : public ParentType
{
public:
	using Type = ParentType::Type;

	constexpr uint32_t get_storage_nbits () const
	{
		return storage_nbits;
	}

	static_assert(storage_nbits == (sizeof(Type)*8));

	constexpr Bitset (const Bitset& other)
	{
		this->storage = other.storage;
	}

	constexpr Bitset (const Type v)
	{
		this->storage = v;
	}

	template <typename T>
	constexpr Bitset& operator= (const Bitset& other)
	{
		this->storage = other.storage;
		return *this;
	}

	template <typename T>
	constexpr Bitset& operator= (const Type v)
	{
		this->storage = v;
		return *this;
	}

	constexpr Type underlying () const
	{
		return this->storage;
	}

	constexpr bool operator[] (const std::size_t pos) const
	{
		return (this->storage >> pos) & 0x01;
	}

	constexpr Bitset operator() (const std::size_t ini, const std::size_t length) const
	{
		return Bitset( extract_bits(this->storage, ini, length) );
	}

	constexpr Bitset range (const std::size_t ini, const std::size_t end) const
	{
		return Bitset( extract_bits(this->storage, ini, end-ini+1) );
	}

	constexpr Bitset extract (const std::size_t ini, const std::size_t length) const
	{
		return Bitset( extract_bits(this->storage, ini, length) );
	}

	constexpr Bitset& operator&= (const Bitset& other)
	{
		this->storage &= other.storage;
		return *this;
	}

	constexpr Bitset& operator&= (const Type v)
	{
		this->storage &= v;
		return *this;
	}

	constexpr Bitset& operator|= (const Bitset& other)
	{
		this->storage |= other.storage;
		return *this;
	}

	constexpr Bitset& operator|= (const Type v)
	{
		this->storage |= v;
		return *this;
	}

	constexpr Bitset& operator^= (const Bitset& other)
	{
		this->storage ^= other.storage;
		return *this;
	}

	constexpr Bitset& operator^= (const Type v)
	{
		this->storage ^= v;
		return *this;
	}

	constexpr Bitset operator~ () const
	{
		return Bitset(~this->storage);
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif