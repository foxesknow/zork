#pragma once

#include<map>

#include <Zork\AddressSpace.h>
#include <Zork\StringReader.h>

namespace zork
{

class Story
{
private:
	AddressSpace m_AddressSpace;
	
	Address m_StaticBase;
	Address m_AbbereviationTableBase;
	Address m_DictionaryLocation;

	std::map<int,std::string> m_Abbreviations;


private:
	void buildAbbreviationCache();
	const std::string &getAbbreviation(int id)const;
	std::string readAbbreviation(int addreviationNumber)const;

	void buildDictionary();

	Address increaseByteAddress(Address address, int amount)const
	{
		auto offset=sizeof(Byte)*amount;
		return static_cast<Address>(address+offset);
	}

	Address resolveWordAddress(Address address)const
	{
		return address*2;
	}

	void resolveCharacter(std::string &text, const char *&alphabet, StringReader &reader)const;

public:
	Story(AddressSpace &&addressSpace);

	// For now...
	

	std::string readString(Address address)const;
	std::string readString(StringReader &reader)const;
	
};


} // end of namespace