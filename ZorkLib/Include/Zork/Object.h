#pragma once

#include <vector>
#include <tuple>

#include <Zork\CoreTypes.h>
#include <Zork\AddressSpace.h>
#include <Zork\PropertyBlock.h>

namespace zork
{

class Object
{
private:
	AddressSpace &m_AddressSpace;
	Address m_Address;
	bool m_V3OrLess;

	Object(const Object&)=delete;
	Object &operator=(const Object&)=delete;
	Object &operator=(Object&&)=delete;

	std::tuple<int,Byte> getFlagLocation(int flagID)const
	{
		int byte=flagID/8;
		Byte offset=1 << (7-(flagID & 7));

		return std::make_tuple(byte,offset);
	}

	Address getPropertyBlockBase()const
	{
		auto base=getNameAddress();
		
		// The length is the number of WORDS that make up the name
		auto nameLength=m_AddressSpace.readByte(base-1);

		return increaseWordAddress(base,nameLength);
	}

	std::tuple<Address,int,int,bool> getPropertyBlockInfo(Address address)const;

public:
	Object(AddressSpace &addressSpace, Address address) : m_AddressSpace(addressSpace), m_Address(address)
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

	bool getFlag(int flagID)const
	{
		auto location=getFlagLocation(flagID);
		Address address=m_Address+std::get<0>(location);
		Byte mask=std::get<1>(location);
		
		auto value=m_AddressSpace.readByte(address);
		return (value & mask)!=0;
	}

	void setFlag(int flagID, bool state)
	{
		auto location=getFlagLocation(flagID);
		Address address=m_Address+std::get<0>(location);
		Byte mask=std::get<1>(location);
		
		auto value=m_AddressSpace.readByte(address);
		if(state)
		{
			value|=mask;
		}
		else
		{
			value&=~mask;
		}

		m_AddressSpace.writeByte(address,value);
	}

	PropertyBlock getPropertyBlock(int index)const;

	std::vector<PropertyBlock> getAllPropertyBlocks()const;
};

} // end of namespace