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

	std::map<int,std::string> m_Abbreviations;


private:
	const std::string &getAbbreviation(int id)const;

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
	
	std::string readAbbreviation(int addreviationNumber)const;

public:
	Story(AddressSpace &&addressSpace);

	// For now...
	void buildAbbreviationCache();

	std::string readString(Address address)const;
	
};


} // end of namespace