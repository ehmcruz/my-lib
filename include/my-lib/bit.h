#ifndef __MY_LIB_BIT_HEADER_H__
#define __MY_LIB_BIT_HEADER_H__

#include <ostream>

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

template <std::size_t storage_nbits>
class BitSetStorage__;

template<>
class BitSetStorage__<8>
{
public:
	using Type = uint8_t;
protected:
	Type storage;
};

template<>
class BitSetStorage__<16>
{
public:
	using Type = uint16_t;
protected:
	Type storage;
};

template<>
class BitSetStorage__<32>
{
public:
	using Type = uint32_t;
protected:
	Type storage;
};

template<>
class BitSetStorage__<64>
{
public:
	using Type = uint64_t;
protected:
	Type storage;
};

consteval std::size_t calc_bit_set_storage_nbits__ (const std::size_t nbits)
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

template <std::size_t nbits,
          std::size_t storage_nbits = calc_bit_set_storage_nbits__(nbits),
		  typename ParentType = BitSetStorage__<storage_nbits>
		  >
class BitSet : public ParentType
{
public:
	using Type = ParentType::Type;

	static consteval std::size_t get_storage_nbits ()
	{
		return storage_nbits;
	}

	static_assert(storage_nbits == (sizeof(Type)*8));

	static consteval std::size_t size ()
	{
		return nbits;
	}

	BitSet () = default;

	constexpr BitSet (const BitSet& other)
	{
		this->storage = other.storage;
	}

	constexpr BitSet (const Type v)
	{
		this->storage = v;
	}

	template <typename T>
	constexpr BitSet& operator= (const BitSet& other)
	{
		this->storage = other.storage;
		return *this;
	}

	template <typename T>
	constexpr BitSet& operator= (const Type v)
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

	constexpr BitSet operator() (const std::size_t ini, const std::size_t length) const
	{
		return BitSet( extract_bits(this->storage, ini, length) );
	}

	constexpr BitSet range (const std::size_t ini, const std::size_t end) const
	{
		return BitSet( extract_bits(this->storage, ini, end-ini+1) );
	}

	constexpr BitSet extract (const std::size_t ini, const std::size_t length) const
	{
		return BitSet( extract_bits(this->storage, ini, length) );
	}

	constexpr BitSet& operator&= (const BitSet& other)
	{
		this->storage &= other.storage;
		return *this;
	}

	constexpr BitSet& operator&= (const Type v)
	{
		this->storage &= v;
		return *this;
	}

	constexpr BitSet& operator|= (const BitSet& other)
	{
		this->storage |= other.storage;
		return *this;
	}

	constexpr BitSet& operator|= (const Type v)
	{
		this->storage |= v;
		return *this;
	}

	constexpr BitSet& operator^= (const BitSet& other)
	{
		this->storage ^= other.storage;
		return *this;
	}

	constexpr BitSet& operator^= (const Type v)
	{
		this->storage ^= v;
		return *this;
	}

	constexpr BitSet operator~ () const
	{
		return BitSet(~this->storage);
	}
};

// ---------------------------------------------------

template <std::size_t nbits>
std::ostream& operator << (std::ostream& o, const BitSet<nbits>& bitset)
{
	for (int32_t i = bitset.size(); i >= 0; i--)
		o << bitset[i];
	return o;
}

// ---------------------------------------------------

} // end namespace Mylib

#endif