#include <utility>

#include <Zork\Story.h>

#include<iostream>

namespace zork
{

Story::Story(AddressSpace &&addressSpace) : m_AddressSpace(std::move(addressSpace))
{
	m_StaticBase=m_AddressSpace.readWord(0x0e);
	m_AbbereviationTableBase=m_AddressSpace.readWord(0x18);
	m_DictionaryLocation=m_AddressSpace.readWord(0x08);

	buildAbbreviationCache();
	buildDictionary();
}

void Story::buildAbbreviationCache()
{
	for(int i=0; i<96; i++)
	{
		auto value=readAbbreviation(i);
		m_Abbreviations[i]=value;
	}
}

std::string Story::readAbbreviation(int addreviationNumber)const
{
	auto abbreviationAddress=increaseWordAddress(m_AbbereviationTableBase,addreviationNumber);
	auto stringPointer=resolveWordAddress(m_AddressSpace.readWord(abbreviationAddress));

	return readString(stringPointer);
}

const std::string &Story::getAbbreviation(int id)const
{
	auto it=m_Abbreviations.find(id);
	if(it==m_Abbreviations.end()) 
	{
		throw Exception("could not find abbreviation");
	}

	return it->second;
}


void Story::buildDictionary()
{
	Byte numberOfBytes=m_AddressSpace.readByte(m_DictionaryLocation);

	auto address=m_DictionaryLocation+1;
	
	std::string wordSeparators;
	for(Byte i=0; i<numberOfBytes; i++, address++)
	{
		auto zscii=m_AddressSpace.readByte(address);
		wordSeparators+=static_cast<char>(zscii);
	}

	// address now points to the "entry length" field
	auto entryLength=m_AddressSpace.readByte(address++);
	auto numberOfEntries=m_AddressSpace.readWord(address);

	auto entryAddress=increaseWordAddress(address,1);

	for(Word i=0; i<numberOfEntries; i++, entryAddress+=entryLength)
	{
		StringReader reader(&m_AddressSpace,entryAddress);
		auto text=readString(reader);

		std::cout << text << std::endl;
	}
}


} // end of namespace