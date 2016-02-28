#pragma once

#include <vector>

#include <Zork\CoreTypes.h>

namespace zork
{

class Object
{
private:
	std::vector<Byte> m_Attributes;
	Word m_Parent;
	Word m_Sibling;
	Word m_Child;

	Address m_PropetiesAddress;

public:

	void addAttribute(Byte byte)
	{
		m_Attributes.push_back(byte);
	}

	void setParent(Word parent)
	{
		m_Parent=parent;
	}

	Word getParent()const
	{
		return m_Parent;
	}

	void setSibling(Word sibling)
	{
		m_Sibling=sibling;
	}

	Word getSibling()const
	{
		return m_Sibling;
	}

	void setChild(Word child)
	{
		m_Child=child;
	}

	Word getChild()const
	{
		return m_Child;
	}

	void setPropertiesAddres(Address propertiesAddress)
	{
		m_PropetiesAddress=propertiesAddress;
	}

	Address getPropertiesAddress()const
	{
		return m_PropetiesAddress;
	}

};

} // end of namespace