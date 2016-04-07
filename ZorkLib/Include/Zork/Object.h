#pragma once

#include <vector>
#include <tuple>

#include <Zork\CoreTypes.h>
#include <Zork\AddressSpace.h>
#include <Zork\PropertyBlock.h>

namespace zork
{

/**
 * Represents an object in the game
 */
class Object
{
private:
	AddressSpace &m_AddressSpace;
	Address m_Address;
	bool m_V3OrLess;
	Word m_ObjectID;

	Object(const Object&)=delete;
	Object &operator=(const Object&)=delete;
	Object &operator=(Object&&)=delete;

	/**
	 * Works out the location of a flag in an array.
	 * Attribute are stored topmost bit first, so flag 0 is bit 7 of the first byte etc
	 */
	std::tuple<int,Byte> getFlagLocation(int flagID)const
	{
		int byte = flagID / 8;
		Byte offset = 1 << (7 - (flagID & 7));

		return std::make_tuple(byte, offset);
	}

	Address getPropertyBlockBase()const
	{
		auto base = getNameAddress();
		
		// The length is the number of WORDS that make up the name
		auto nameLength = m_AddressSpace.readByte(base - 1);

		return increaseWordAddress(base, nameLength);
	}

	/**
	 * Returns information on the property block at the given address.
	 * The tuple is (StartOfData, SizeOfData, PropertyNumber, Terminator)
	 */
	std::tuple<Address,int,Word,bool> getPropertyBlockInfo(Address address)const;

	void setParent(Word parentID)
	{
		Address address = (m_V3OrLess ? m_Address + 4 : m_Address + 6);
		if(m_V3OrLess)
		{
			m_AddressSpace.writeByte(address, static_cast<Byte>(parentID));
		}
		else
		{
			m_AddressSpace.writeWord(address, parentID);
		}
	}

	void setChild(Word childID)
	{
		Address address = (m_V3OrLess ? m_Address + 6 : m_Address + 10);
		if(m_V3OrLess)
		{
			m_AddressSpace.writeByte(address,static_cast<Byte>(childID));
		}
		else
		{
			m_AddressSpace.writeWord(address,childID);
		}
	}

	void setSibling(Word siblingID)
	{
		Address address = (m_V3OrLess ? m_Address + 5 : m_Address + 8);
		if(m_V3OrLess)
		{
			m_AddressSpace.writeByte(address, static_cast<Byte>(siblingID));
		}
		else
		{
			m_AddressSpace.writeWord(address, siblingID);
		}
	}

public:
	Object(AddressSpace &addressSpace, Address address, Word objectID) : m_AddressSpace(addressSpace), m_Address(address), m_ObjectID(objectID)
	{
		m_V3OrLess = versionThreeOrLess(addressSpace.readByte(0), true, false);
	}

	Object(Object &&rhs) : m_AddressSpace(rhs.m_AddressSpace), m_Address(rhs.m_Address), m_V3OrLess(rhs.m_V3OrLess)
	{
	}

	/** Makes object the child of this */
	void insertObject(Object &childObject)
	{
		Word previousChild=getChild();

		// The child of this will be set to childObject (so the parent of childObject will be this)
		childObject.setParent(m_ObjectID);
		setChild(childObject.getObjectID());

		// The sibling of childObject will be whatever was the previous sibling.
		// This was the effect of patching the child into the chain of siblings
		childObject.setSibling(previousChild);
	}

	Word getObjectID()const
	{
		return m_ObjectID;
	}

	/** Returns the number of the parent, or 0 if no parent */
	Word getParent()const
	{
		Address address = (m_V3OrLess ? m_Address + 4 : m_Address + 6);
		return m_V3OrLess ? m_AddressSpace.readByte(address) : m_AddressSpace.readWord(address);
	}

	/** Returns the number of the sibling, or 0 if no sibling */
	Word getSibling()const
	{
		Address address = (m_V3OrLess ? m_Address + 5 : m_Address + 8);
		return m_V3OrLess ? m_AddressSpace.readByte(address) : m_AddressSpace.readWord(address);
	}

	/** Returns the number of the child, or 0 if no child */
	Word getChild()const
	{
		Address address = (m_V3OrLess ? m_Address + 6 : m_Address + 10);
		return m_V3OrLess ? m_AddressSpace.readByte(address) : m_AddressSpace.readWord(address);
	}

	/** The address of the object properties */
	Address getPropertiesAddress()const
	{
		Address address = (m_V3OrLess ? m_Address + 7 : m_Address + 12);
		return m_V3OrLess ? m_AddressSpace.readWord(address) : m_AddressSpace.readWord(address);
	}

	/** The address of the name of the object */
	Address getNameAddress()const
	{
		auto address = getPropertiesAddress();

		// The next byte is the number of WORDS in the string
		address++;
		return address;
	}

	/** Gets the value of the specified flag */
	bool getFlag(int flagID)const
	{
		auto location = getFlagLocation(flagID);
		Address address = m_Address + std::get<0>(location);
		Byte mask = std::get<1>(location);
		
		auto value = m_AddressSpace.readByte(address);
		return (value & mask) != 0;
	}

	/** Sets the flag to the specified state */
	void setFlag(int flagID, bool state)
	{
		auto location = getFlagLocation(flagID);
		Address address = m_Address+std::get<0>(location);
		Byte mask = std::get<1>(location);
		
		auto value = m_AddressSpace.readByte(address);
		if(state)
		{
			value |= mask;
		}
		else
		{
			value &= ~mask;
		}

		m_AddressSpace.writeByte(address, value);
	}

	/** Returns the specified property block */
	PropertyBlock getPropertyBlock(int index)const;

	/** Returns all the property blocks for the object */
	std::vector<PropertyBlock> getAllPropertyBlocks()const;

	/** Returns the size of an object entry */
	static int getEntrySize(Byte version)
	{
		return versionThreeOrLess(version, 9, 14);
	}
};

} // end of namespace