#pragma once

#include <utility>
#include <string>
#include <Zork\CoreTypes.h>

namespace zork
{

class AddressSpace
{
private:
	Byte *m_Memory;
	size_t m_Size;

	void checkIsValid(Address address)const
	{
		if(address>=m_Size)
		{
			// TODO: panic()!
		}
	}

	AddressSpace(const AddressSpace&)=delete;
	AddressSpace &operator=(const AddressSpace &rhs)=delete;

public:
	explicit AddressSpace(size_t size) : m_Size(size)
	{
		m_Memory=new Byte[size];
	}	

	AddressSpace(AddressSpace &&rhs) : m_Memory(nullptr), m_Size(0)
	{
		std::swap(m_Memory,rhs.m_Memory);
		std::swap(m_Size,rhs.m_Size);
	}

	~AddressSpace()
	{
		delete []m_Memory;
	}

	AddressSpace &operator=(AddressSpace &&rhs)
	{
		if(this!=&rhs)
		{
			std::swap(m_Memory,rhs.m_Memory);
			std::swap(m_Size,rhs.m_Size);
		}

		return *this;
	}

	/** Returns a pointer to the start of the address space */
	Byte *base()
	{
		return m_Memory;
	}

	/** Returns a pointer to the start of the address space */
	const Byte *base()const
	{
		return m_Memory;
	}

	/** Returns the size of the address space */
	size_t size()const
	{
		return m_Size;
	}

	/** Returns a pointer to the specified address */
	const Byte *pointerTo(Address address)const
	{
		checkIsValid(address);
		return m_Memory+address;
	}

	/** Reads a byte */
	Byte readByte(Address address)const
	{
		checkIsValid(address);

		return m_Memory[address];
	}

	/* Writes a byte */
	void writeByte(Address address, Byte value)
	{
		checkIsValid(address);

		m_Memory[address]=value;
	}

	/** Reads a word */
	Word readWord(Address address)const
	{
		checkIsValid(address);

		auto high=m_Memory[address];
		auto low=m_Memory[address+1];
		auto value=(high*256)+low;

		return static_cast<Word>(value);
	}

	/** Writes a word */
	void writeWord(Address address, Word word)
	{
		checkIsValid(address);

		Byte low=word & 0xff;
		Byte high=word >> 8;
		
		m_Memory[address]=high;
		m_Memory[address+1]=low;
	}


};

AddressSpace loadAddressSpaceFromFile(const std::string &filename);

} // end of namespace