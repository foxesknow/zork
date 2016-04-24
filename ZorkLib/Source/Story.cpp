#include <utility>

#include <iostream>
#include <sstream>

#include <Zork\Story.h>
#include <Zork\Object.h>

namespace zork
{

Story::Story(AddressSpace &&addressSpace, std::shared_ptr<Console> console) : m_AddressSpace(std::move(addressSpace)), m_Console(console)
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
	
	for(Byte i = 0; i < numberOfBytes; i++, address++)
	{
		auto zscii = m_AddressSpace.readByte(address);
		m_WordSeparators += static_cast<char>(zscii);
	}

	// address now points to the "entry length" field
	auto entryLength = m_AddressSpace.readByte(address++);
	auto numberOfEntries = m_AddressSpace.readWord(address);

	auto entryAddress = increaseWordAddress(address, 1);

	for(Word i = 0; i < numberOfEntries; i++, entryAddress += entryLength)
	{
		AddressSpaceZsciiReader reader(m_AddressSpace, entryAddress);
		auto text = readString(reader);
		m_Dictionary.insert(DictionaryEntry(entryAddress, text));
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

void Story::unlinkObject(Word objectID)
{
	auto object = getObject(objectID);
	if(object.getParent() == 0) return;

	auto parent = getObject(object.getParent());

	auto nextSibling = object.getSibling();
	auto firstSibling = parent.getChild();

	object.setParent(0);
	object.setSibling(0);

	if(firstSibling == objectID)
	{
		// Easy, we're removing the first child, so the parent's child property is just the next sibling
		object.setChild(nextSibling);
	}
	else
	{
		// We need to remove the object from the list of siblings

		auto sibID = firstSibling;
		auto lastSibID = sibID;

		do
		{
			lastSibID = sibID;
			sibID = getObject(sibID).getSibling();
		}while(sibID != objectID);

		getObject(lastSibID).setSibling(nextSibling);
	}
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

Address Story::expandPackedRoutineAddress(Address address)const
{
	switch(m_Version)
	{
		case 1:
		case 2:
		case 3:
			return address * 2;

		case 4:
		case 5:
			return address * 4;

		case 6:
		case 7:
		{
			// We need to use the routine offset
			Word routineOffset = m_AddressSpace.readWord(0x28);
			return (address * 4) + (8 * routineOffset);
		}

		case 8:
			return address * 8;

		default:
			panic("unsupported version");
			return 0;
	}
}

Address Story::expandPackedStringAddress(Address address)const
{
	switch(m_Version)
	{
		case 1:
		case 2:
		case 3:
			return address * 2;

		case 4:
		case 5:
			return address * 4;

		case 6:
		case 7:
		{
			// We need to use the string offset
			Word routineOffset = m_AddressSpace.readWord(0x2a);
			return (address * 4) + (8 * routineOffset);
		}

		case 8:
			return address * 8;

		default:
			panic("unsupported version");
			return 0;
	}
}


int Story::getFileSize() const
{
	int size = m_AddressSpace.readWord(0x1a);

	// The size is actually divided by a constant, depending on the version
	switch(m_Version)
	{
		case 1:
		case 2:
		case 3:
			return size * 2;
	
		case 4:
		case 5:
			return size * 4;

		default:
			return size * 8;
	}
}

void Story::ThrowNotImplemented(const OpcodeDetails opcodeDetails) const
{
	std::stringstream stream;
	stream << "not implemented!  " << opcodeDetails;
	throw Exception(stream.str());
}


} // end of namespace