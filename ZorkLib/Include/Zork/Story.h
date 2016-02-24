#pragma once

#include<map>

#include <Zork\AddressSpace.h>

namespace zork
{

class Story
{
private:
	AddressSpace m_AddressSpace;
	
	address_t m_StaticBase;
	address_t m_AbbereviationTableBase;

	std::map<word_t,std::string> m_Abbreviations;


private:
	address_t increaseByteAddress(address_t address, int amount)const
	{
		auto offset=sizeof(byte_t)*amount;
		return static_cast<address_t>(address+offset);
	}

	address_t increaseWordAddress(address_t address, int amount)const
	{
		auto offset=sizeof(word_t)*amount;
		return static_cast<address_t>(address+offset);
	}

	address_t resolveWordAddress(address_t address)const
	{
		return address*2;
	}

	void resolveCharacter(std::string &text, const char *&alphabet, int character)const;
	
	std::string readAbbreviation(int addreviationNumber)const;

public:
	Story(AddressSpace &&addressSpace);

	// For now...
	void buildAbbreviationCache();
	
};


} // end of namespace