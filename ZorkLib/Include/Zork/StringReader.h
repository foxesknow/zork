#pragma once

#include <Zork\AddressSpace.h>

namespace zork
{

class StringReader
{
private:
	const AddressSpace *m_AddressSpace;
	Address m_Address;

	int m_Chars[3];
	int m_Index;

	bool m_AtEnd;

private:
	void update(Address address)
	{
		auto word=m_AddressSpace->readWord(address);

		m_Chars[2]=word & 0x1f;
		word >>= 5;

		m_Chars[1]=word & 0x1f;
		word >>= 5;

		m_Chars[0]=word & 0x1f;
		word >>= 5;
		
		m_AtEnd=word & 1;
		m_Index=0;
	}


public:
	StringReader(const AddressSpace *addressSpace, Address normalizedAddress) : m_AddressSpace(addressSpace), m_Address(normalizedAddress)
	{
		update(m_Address);
	}

	bool moveNext()
	{
		if(m_AtEnd && m_Index==2)
		{
			return false;
		}

		m_Index++;

		if(m_Index==3)
		{
			// Read the next word;
			m_Address=increaseWordAddress(m_Address,1);
			update(m_Address);
		}

		return true;
	}

	int getCurrent()const
	{
		return m_Chars[m_Index];
	}
};

} // end of namespace
