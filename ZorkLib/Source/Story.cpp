#include <utility>

#include <Zork\Story.h>

namespace
{

const char alphabetTable_A0[]=
{
	'_', '?', '?', '?', '?', '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' 
};

const char alphabetTable_A1[]=
{
	'_', '?', '?', '?', '?', '?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' 
};

const char alphabetTable_A2[]=
{
	'_', '?', '?', '?', '?', '?', '\0', '\n', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '.', ',', '!', '?', '_', '#', '`', '"', '/', '\\', '-', ':', '(', ')' 
};

} // end of namespace

namespace zork
{

Story::Story(AddressSpace &&addressSpace) : m_AddressSpace(std::move(addressSpace))
{
	m_StaticBase=m_AddressSpace.ReadWord(0x0e);

	m_AbbereviationTableBase=m_AddressSpace.ReadWord(0x18);
}

void Story::buildAbbreviationCache()
{
	for(Word i=0; i<96; i++)
	{
		auto value=readAbbreviation(i);
		m_Abbreviations[i]=value;
	}
}

std::string Story::readAbbreviation(int addreviationNumber)const
{
	auto abbreviationAddress=increaseWordAddress(m_AbbereviationTableBase,addreviationNumber);
	auto stringPointer=resolveWordAddress(m_AddressSpace.ReadWord(abbreviationAddress));

	std::string value;

	auto alphabetTable=alphabetTable_A0;

	while(true)
	{
		auto word=m_AddressSpace.ReadWord(stringPointer);

		auto c3=word & 0x1f;
		word >>= 5;

		auto c2=word & 0x1f;
		word >>= 5;

		auto c1=word & 0x1f;
		word >>= 5;
		
		auto atEnd=word & 1;

		resolveCharacter(value,alphabetTable,c1);
		resolveCharacter(value,alphabetTable,c2);
		resolveCharacter(value,alphabetTable,c3);		
		
		if(atEnd) break;

		stringPointer=increaseWordAddress(stringPointer,1);
	}

	return value;
}

void Story::resolveCharacter(std::string &text, const char *&alphabet, int character)const
{
	if(character>5)
	{
		// It's not a shift character
		char c=alphabet[character];
		if(c) text+=c;
		alphabet=alphabetTable_A0;
	}
	else
	{
		// It's a shift
		switch(character)
		{
			case 0:
				text+=' ';
				break;

			case 1:
				break;

			case 2:
				break;

			case 3:
				break;

			case 4:
				alphabet=alphabetTable_A1;
				break;

			case 5:
				alphabet=alphabetTable_A2;
				break;

			default:
				break;
		}
	}
}

} // end of namespace