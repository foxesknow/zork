#pragma once

#include<map>

#include <Zork\AddressSpace.h>

namespace zork
{

class Story
{
private:
	AddressSpace m_AddressSpace;
	
	Address m_StaticBase;
	Address m_AbbereviationTableBase;

	std::map<Word,std::string> m_Abbreviations;


private:
	Address increaseByteAddress(Address address, int amount)const
	{
		auto offset=sizeof(Byte)*amount;
		return static_cast<Address>(address+offset);
	}

	Address increaseWordAddress(Address address, int amount)const
	{
		auto offset=sizeof(Word)*amount;
		return static_cast<Address>(address+offset);
	}

	Address resolveWordAddress(Address address)const
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