#include <utility>

#include <iostream>

#include <Zork\Story.h>
#include <Zork\Object.h>

namespace zork
{

Story::Story(AddressSpace &&addressSpace) : m_AddressSpace(std::move(addressSpace))
{
	// NOTE: header layout on page 61

	m_Version = m_AddressSpace.readByte(0);
	m_StaticBase = m_AddressSpace.readWord(0x0e);
	m_GlobalVariablesTable = m_AddressSpace.readWord(0x0c);

	// NOTE: For now
	m_PC = 0;

	buildAbbreviationCache();
	buildDictionary();
}

void Story::buildAbbreviationCache()
{
	for(int i = 0; i < 96; i++)
	{
		auto value = readAbbreviation(i);
		m_Abbreviations[i] = value;
	}
}

std::string Story::readAbbreviation(int addreviationNumber)const
{
	auto abbereviationTableBase = m_AddressSpace.readWord(0x18);

	auto abbreviationAddress = increaseWordAddress(abbereviationTableBase, addreviationNumber);
	auto stringPointer = 2 * m_AddressSpace.readWord(abbreviationAddress);

	return readString(stringPointer);
}

const std::string &Story::getAbbreviation(int id)const
{
	auto it = m_Abbreviations.find(id);
	if(it == m_Abbreviations.end()) 
	{
		throw Exception("could not find abbreviation");
	}

	return it->second;
}


void Story::buildDictionary()
{
	auto dictionaryLocation = m_AddressSpace.readWord(0x08);
	Byte numberOfBytes = m_AddressSpace.readByte(dictionaryLocation);

	auto address = dictionaryLocation+1;
	
	std::string wordSeparators;
	for(Byte i = 0; i < numberOfBytes; i++, address++)
	{
		auto zscii = m_AddressSpace.readByte(address);
		wordSeparators += static_cast<char>(zscii);
	}

	// address now points to the "entry length" field
	auto entryLength = m_AddressSpace.readByte(address++);
	auto numberOfEntries = m_AddressSpace.readWord(address);

	auto entryAddress = increaseWordAddress(address, 1);

	for(Word i = 0; i < numberOfEntries; i++, entryAddress += entryLength)
	{
		ZsciiReader reader(m_AddressSpace, entryAddress);
		auto text = readString(reader);
		m_Dictionary.insert(text);
	}
}

Word Story::getDefaultProperty(Word propertyID)const
{
	auto objectTableAddress = m_AddressSpace.readWord(0x0a);
	auto location = increaseWordAddress(objectTableAddress, propertyID);
	return m_AddressSpace.readWord(location);
}

Word Story::getNumberOfDefaultProperties()const
{
	return versionThreeOrLess<Word>(m_Version, 31, 63);
}

Address Story::getObjectTreeBaseAddress()const
{
	auto objectTableAddress = m_AddressSpace.readWord(0x0a);
	const int numberOfDefaults = versionThreeOrLess(m_Version, 31, 63);
	const auto objectTreeAddress = increaseWordAddress(objectTableAddress, numberOfDefaults);

	return objectTreeAddress;
}

Object Story::getObject(Word objectID)const
{
	const auto objectTreeAddress = getObjectTreeBaseAddress();
	const int entrySize = Object::getEntrySize(m_Version);

	// objectIDs are 1 based
	Address objectAddress = objectTreeAddress+((objectID - 1) * entrySize);
	return Object(m_AddressSpace, objectAddress, objectID);
}

Word Story::getNumberOfObjects()const
{
	// There's no explicit count of the number of objects.
	// However, the properties for the first object follow
	// straight after the last object, so we can work it
	// out from this
	
	auto treeBase = getObjectTreeBaseAddress();
	auto lastObject = m_AddressSpace.readWord(treeBase + Object::getEntrySize(m_Version) -2 );
	auto entrySize = Object::getEntrySize(m_Version);

	auto numberOfObjects = (lastObject - treeBase) / entrySize;
	return static_cast<Word>(numberOfObjects);
}

} // end of namespace