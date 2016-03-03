#pragma once

#include <Zork\CoreTypes.h>

namespace zork
{

class PropertyBlock
{
private:
	Address m_Address;
	int m_Size;
	int m_ID;

public:
	PropertyBlock(Address address, int size, int ID) : m_Address(address), m_Size(size), m_ID(ID)
	{
	}

	Address getAddress()const
	{
		return m_Address;
	}

	int getSize()const
	{
		return m_Size;
	}

	int getID()const
	{
		return m_ID;
	}
};

} // end of namespace