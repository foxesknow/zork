#pragma once

#include <map>
#include <set>
#include <vector>

#include <Zork\AddressSpace.h>
#include <Zork\ZsciiReader.h>
#include <Zork\Object.h>

namespace zork
{

class Story
{
private:
	Byte m_Version;

	mutable AddressSpace m_AddressSpace;
	
	Address m_StaticBase;
	Address m_GlobalVariablesTable;

	std::map<int,std::string> m_Abbreviations;
	std::set<std::string> m_Dictionary;

	std::vector<Word> m_PropertyDefaults;


private:
	void buildAbbreviationCache();
	const std::string &getAbbreviation(int id)const;
	std::string readAbbreviation(int addreviationNumber)const;

	void buildDictionary();

	void parseObjectTable();

	Address increaseByteAddress(Address address, int amount)const
	{
		auto offset=sizeof(Byte)*amount;
		return static_cast<Address>(address+offset);
	}

	Address resolveWordAddress(Address address)const
	{
		return address*2;
	}

	void resolveCharacter(std::string &text, const char *&alphabet, ZsciiReader &reader)const;

	Address getObjectTreeBaseAddress()const;

public:
	Story(AddressSpace &&addressSpace);

	// For now...
	std::string readString(Address address)const;
	std::string readString(ZsciiReader &reader)const;

	Object getObject(int objectID)const;

	int getNumberOfObjects()const;
	
};


} // end of namespace