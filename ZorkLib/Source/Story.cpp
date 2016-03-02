#include <utility>

#include <iostream>

#include <Zork\Story.h>
#include <Zork\Object.h>

namespace zork
{

Story::Story(AddressSpace &&addressSpace) : m_AddressSpace(std::move(addressSpace))
{
	// NOTE: header layout on page 61

	m_Version=m_AddressSpace.readByte(0);
	m_StaticBase=m_AddressSpace.readWord(0x0e);

	buildAbbreviationCache();
	buildDictionary();
	parseObjectTable();
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
	auto abbereviationTableBase=m_AddressSpace.readWord(0x18);

	auto abbreviationAddress=increaseWordAddress(abbereviationTableBase,addreviationNumber);
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
	auto dictionaryLocation=m_AddressSpace.readWord(0x08);
	Byte numberOfBytes=m_AddressSpace.readByte(dictionaryLocation);

	auto address=dictionaryLocation+1;
	
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
		StringReader reader(m_AddressSpace,entryAddress);
		auto text=readString(reader);
		m_Dictionary.insert(text);
	}
}

Object Story::getObject(int objectID)const
{
	auto objectTableAddress=m_AddressSpace.readWord(0x0a);
	const int numberOfDefaults=versionThreeOrLess(m_Version,31,63);
	const auto objectTreeAddress=increaseWordAddress(objectTableAddress,numberOfDefaults);

	const int entrySize=versionThreeOrLess(m_Version,9,14);

	// objectIDs are 1 based
	Address objectAddress=objectTreeAddress+((objectID-1)*entrySize);
	return Object(m_AddressSpace,objectAddress);
}

void Story::parseObjectTable()
{
	auto objectTableAddress=m_AddressSpace.readWord(0x0a);

	// First, read the defaults
	const int numberOfDefaults=versionThreeOrLess(m_Version,31,63);
	for(int i=0; i<numberOfDefaults; i++)
	{
		auto address=increaseWordAddress(objectTableAddress,i);
		auto value=m_AddressSpace.readWord(address);
		m_PropertyDefaults.push_back(value);
	}

	// See page 65 of spec

	for(int i=1; i<10; i++)
	{
		Object object=getObject(i);
		auto nameAddress=object.getNameAddress();
		auto name=readString(nameAddress);

		std::cout	<< object.getParent() << " " 
					<< object.getSibling() << " "
					<< object.getChild() << " "
					<< name
					<< std::endl;
	}
}


} // end of namespace