#pragma once

#include <vector>

#include <Zork\CoreTypes.h>

namespace zork
{

class Object
{
private:
	const AddressSpace &m_AddressSpace;
	Address m_Address;
	bool m_V3OrLess;

	Object(const Object&)=delete;
	Object &operator=(const Object&)=delete;
	Object &operator=(Object&&)=delete;

public:
	Object(const AddressSpace &addressSpace, Address address) : m_AddressSpace(addressSpace), m_Address(address)
	{
		m_V3OrLess=versionThreeOrLess(addressSpace.readByte(0),true,false);
	}

	Object(Object &&rhs) : m_AddressSpace(rhs.m_AddressSpace), m_Address(rhs.m_Address), m_V3OrLess(rhs.m_V3OrLess)
	{
	}

	Word getParent()const
	{
		Address address=(m_V3OrLess ? m_Address+4 : m_Address+6);
		return m_V3OrLess ? m_AddressSpace.readByte(address) : m_AddressSpace.readWord(address);
	}

	Word getSibling()const
	{
		Address address=(m_V3OrLess ? m_Address+5 : m_Address+8);
		return m_V3OrLess ? m_AddressSpace.readByte(address) : m_AddressSpace.readWord(address);
	}

	Word getChild()const
	{
		Address address=(m_V3OrLess ? m_Address+6 : m_Address+10);
		return m_V3OrLess ? m_AddressSpace.readByte(address) : m_AddressSpace.readWord(address);
	}

	Address getPropertiesAddress()const
	{
		Address address=(m_V3OrLess ? m_Address+7 : m_Address+12);
		return m_V3OrLess ? m_AddressSpace.readWord(address) : m_AddressSpace.readWord(address);
	}

	Address getNameAddress()const
	{
		auto address=getPropertiesAddress();

		// The next byte is the number of WORDS in the string
		address++;
		return address;
	}

};

} // end of namespace