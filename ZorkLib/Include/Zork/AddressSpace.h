#pragma once

#include <string>
#include <Zork\CoreTypes.h>

namespace zork
{

class AddressSpace
{
private:
	byte *m_Memory;
	size_t m_Size;

public:
	explicit AddressSpace(size_t size) : m_Size(size)
	{
		m_Memory=new byte[size];
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

	byte *base()
	{
		return m_Memory;
	}

	const byte *base()const
	{
		return m_Memory;
	}

	size_t size()const
	{
		return m_Size;
	}

};

AddressSpace loadAddressSpaceFromFile(const std::string &filename);

} // end of namespace