#pragma once

#include <Zork\AddressSpace.h>

namespace zork
{

class ZsciiReader
{
private:
	int m_Chars[3];
	int m_Index;

	bool m_AtEnd;

	ZsciiReader(const ZsciiReader&)=delete;
	ZsciiReader(ZsciiReader&&)=delete;
	ZsciiReader &operator=(const ZsciiReader&)=delete;
	ZsciiReader &operator=(ZsciiReader&&)=delete;

protected:
	ZsciiReader()
	{
	}

	void update(Word word)
	{
		m_Chars[2] = word & 0x1f;
		word >>= 5;

		m_Chars[1]=word & 0x1f;
		word >>= 5;

		m_Chars[0] = word & 0x1f;
		word >>= 5;

		m_AtEnd=word & 1;
		m_Index=0;
	}

	virtual bool readNextWord(Word &nextWord) = 0;

public:
	virtual ~ZsciiReader()
	{
	}

	bool moveNext()
	{
		if(m_AtEnd && m_Index == 2)
		{
			return false;
		}

		m_Index++;

		if(m_Index == 3)
		{
			// Read the next word;
			Word nextWord;
			if(readNextWord(nextWord))
			{
				update(nextWord);
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	int getCurrent() const
	{
		return m_Chars[m_Index];
	}	
};

class AddressSpaceZsciiReader : public ZsciiReader
{
private:
	const AddressSpace &m_AddressSpace;
	Address m_Address;

protected:
	bool readNextWord(Word &nextWord) override
	{
		m_Address = increaseWordAddress(m_Address, 1);
		nextWord = m_AddressSpace.readWord(m_Address);

		return true;
	}

public:
	AddressSpaceZsciiReader(const AddressSpace &addressSpace, Address normalizedAddress) : m_AddressSpace(addressSpace), m_Address(normalizedAddress)
	{
		auto word = m_AddressSpace.readWord(m_Address);
		update(word);
	}

	Address getAddress()const
	{
		return m_Address;
	}
};


} // end of namespace
