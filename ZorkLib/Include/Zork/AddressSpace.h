#pragma once

#include <string>
#include <Zork\CoreTypes.h>

namespace zork
{

class AddressSpace
{
private:
	byte_t *m_Memory;
	size_t m_Size;

	void CheckIsValid(address_t address)const
	{
		if(address>=m_Size)
		{
			// TODO: panic()!
		}
	}

public:
	explicit AddressSpace(size_t size) : m_Size(size)
	{
		m_Memory=new byte_t[size];
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

	byte_t *base()
	{
		return m_Memory;
	}

	const byte_t *base()const
	{
		return m_Memory;
	}

	size_t size()const
	{
		return m_Size;
	}

	byte_t ReadByte(address_t address)const
	{
		CheckIsValid(address);

		return m_Memory[address];
	}

	word_t ReadWord(address_t address)const
	{
		CheckIsValid(address);

		auto high=m_Memory[address];
		auto low=m_Memory[address+1];
		auto value=(high*256)+low;

		return static_cast<word_t>(value);
	}


};

AddressSpace loadAddressSpaceFromFile(const std::string &filename);

} // end of namespace