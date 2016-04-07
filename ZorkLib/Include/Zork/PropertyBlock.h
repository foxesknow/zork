#pragma once

#include <Zork\CoreTypes.h>

namespace zork
{

class PropertyBlock
{
private:
	Address m_Address;
	int m_Size;
	Word m_ID;

public:
	PropertyBlock(Address address, int size, Word ID) : m_Address(address), m_Size(size), m_ID(ID)
	{
	}

	/** The address of the block */
	Address getAddress()const
	{
		return m_Address;
	}

	/** The size of the block */
	int getSize()const
	{
		return m_Size;
	}

	/** The property block number(ID) */
	Word getID()const
	{
		return m_ID;
	}
};

} // end of namespace