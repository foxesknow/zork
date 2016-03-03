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

	Byte *base()
	{
		return m_Memory;
	}

	const Byte *base()const
	{
		return m_Memory;
	}

	size_t size()const
	{
		return m_Size;
	}

	const Byte *pointerTo(Address address)const
	{
		checkIsValid(address);
		return m_Memory+address;
	}

	Byte readByte(Address address)const
	{
		checkIsValid(address);

		return m_Memory[address];
	}

	void writeByte(Address address, Byte value)
	{
		checkIsValid(address);

		m_Memory[address]=value;
	}

	Word readWord(Address address)const
	{
		checkIsValid(address);

		auto high=m_Memory[address];
		auto low=m_Memory[address+1];
		auto value=(high*256)+low;

		return static_cast<Word>(value);
	}


};

AddressSpace loadAddressSpaceFromFile(const std::string &filename);

} // end of namespace