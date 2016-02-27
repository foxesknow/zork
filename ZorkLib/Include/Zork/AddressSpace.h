#pragma once

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

public:
	explicit AddressSpace(size_t size) : m_Size(size)
	{
		m_Memory=new Byte[size];
	}

	AddressSpace(AddressSpace &&rhs)
	{
		m_Memory=rhs.m_Memory;
		m_Size=rhs.m_Size;

		rhs.m_Memory=nullptr;
		rhs.m_Size=0;
	}

	~AddressSpace()
	{
		delete []m_Memory;
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