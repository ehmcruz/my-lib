#ifndef __MY_LIB_BIT_HEADER_H__
#define __MY_LIB_BIT_HEADER_H__

#include <ostream>

#include <cstdint>

#include <my-lib/std.h>

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

/*
	BitSet is based on std::bitset.
	Differences:
		- Only works for up tp 64 bits, but it is faster.
		- Allows extraction and set of range of bits.
*/

template <std::size_t nbits,
          std::size_t storage_nbits = calc_bit_set_storage_nbits__(nbits),
		  typename ParentType = BitSetStorage__<storage_nbits>
		  >
class BitSet : public ParentType
{
public:
	using Type = ParentType::Type;

	// --------------------------

	class reference
	{
	private:
		BitSet& bitset;
		const std::size_t pos;
		const std::size_t length;
	public:
		reference () = delete;
		
		reference (BitSet& bitset_, const std::size_t pos_, const std::size_t length_)
			: bitset(bitset_), pos(pos_), length(length_)
		{
		}

		reference (const reference& other)
			: bitset(other.bitset), pos(other.pos), length(other.length)
		{
		}

		reference& operator= (const Type value)
		{
			this->bitset.set(this->pos, this->length, value);
			return *this;
		}

		reference& operator= (const reference& other)
		{
			mylib_assert_exception_msg(this->length == other.length, "The length of both references must be the same. Given ", this->length, " and ", other.length, ".");
			const Type value = other.bitset.extract_underlying(other.pos, other.length);
			this->bitset.set(this->pos, this->length, value);
			return *this;
		}

		template <std::size_t nbits_other>
		reference& operator= (const BitSet<nbits_other>& other)
		{
			mylib_assert_exception_msg(this->length == other.size(), "The length of both references must be the same. Given ", this->length, " and ", other.size(), ".");
			this->bitset.set(this->pos, this->length, other.underlying());
			return *this;
		}

		Type operator~ () const
		{
			const Type mask = (1 << this->length) - 1;
			return (~this->bitset.extract_underlying(this->pos, this->length)) & mask;
		}

		operator Type() const
		{
			return this->bitset.extract_underlying(this->pos, this->length);
		}
	};

	// --------------------------

	static consteval std::size_t get_storage_nbits ()
	{
		return storage_nbits;
	}

	static_assert(storage_nbits == (sizeof(Type)*8));

	static consteval std::size_t size ()
	{
		return nbits;
	}

	// --------------------------

	constexpr BitSet ()
	{
		this->storage = 0;
	}

	constexpr BitSet (const BitSet& other)
	{
		this->storage = other.storage;
	}

	constexpr BitSet (const Type v)
	{
		this->storage = v;
	}

	// --------------------------

	constexpr BitSet& operator= (const BitSet& other)
	{
		this->storage = other.storage;
		return *this;
	}

	constexpr BitSet& operator= (const Type v)
	{
		this->storage = v;
		return *this;
	}

	// --------------------------

	constexpr Type underlying () const
	{
		return this->storage;
	}

	// --------------------------

	constexpr bool operator[] (const std::size_t pos) const
	{
		return (this->storage >> pos) & 0x01;
	}

	reference operator[] (const std::size_t pos)
	{
		return reference(*this, pos, 1);
	}

	// --------------------------

	constexpr Type operator() (const std::size_t ini, const std::size_t length) const
	{
		return extract_bits(this->storage, ini, length);
	}

	reference operator() (const std::size_t ini, const std::size_t length)
	{
		return reference(*this, ini, length);
	}

	// --------------------------

	constexpr BitSet range (const std::size_t ini, const std::size_t end) const
	{
		return BitSet( extract_bits(this->storage, ini, end-ini+1) );
	}

	constexpr Type range_underlying (const std::size_t ini, const std::size_t end) const
	{
		return extract_bits(this->storage, ini, end-ini+1);
	}

	constexpr BitSet extract (const std::size_t ini, const std::size_t length) const
	{
		return BitSet( extract_bits(this->storage, ini, length) );
	}

	constexpr Type extract_underlying (const std::size_t ini, const std::size_t length) const
	{
		return extract_bits(this->storage, ini, length);
	}

	// --------------------------

	constexpr void set_by_range (const std::size_t ini, const std::size_t end, const Type v)
	{
		this->storage = set_bits(this->storage, ini, end-ini+1, v);
	}

	constexpr void set (const std::size_t ini, const std::size_t length, const Type v)
	{
		this->storage = set_bits(this->storage, ini, length, v);
	}

	// --------------------------

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

	// --------------------------

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

	// --------------------------

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

	// --------------------------

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