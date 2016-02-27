#include <utility>

#include <Zork\Story.h>

namespace
{

const char alphabetTable_A0[]=
{
	'_', '?', '?', '?', '?', '?', 'a', 'b',
	'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
	's', 't', 'u', 'v', 'w', 'x', 'y', 'z' 
};

const char alphabetTable_A1[]=
{
	'_', '?', '?', '?', '?', '?', 'A', 'B',
	'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 
	'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' 
};

const char alphabetTable_A2[]=
{
	'_', '?', '?', '?', '?', '?', '\0', '\n',
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '.', ',', '!', '?', '_', '#',
	 '`', '"', '/', '\\', '-', ':', '(', ')' 
};

} // end of namespace


namespace zork
{

std::string Story::readString(Address normalizedAddress)const
{
	StringReader reader(&m_AddressSpace,normalizedAddress);
	return readString(reader);	
}

std::string Story::readString(StringReader &reader)const
{
	std::string value;

	auto alphabetTable=alphabetTable_A0;

	do
	{
		resolveCharacter(value,alphabetTable,reader);
	}while(reader.moveNext());

	return value;
}

void Story::resolveCharacter(std::string &text, const char *&alphabet, StringReader &reader)const
{
	int character=reader.getCurrent();

	if(character>5)
	{
		if(character==6 && alphabet==alphabetTable_A2)
		{
			reader.moveNext();
			auto high=reader.getCurrent();

			reader.moveNext();
			auto low=reader.getCurrent();

			auto zsciiCode=(high<<5)+low;
			char c=static_cast<char>(zsciiCode);
			if(c) text+=c;
		}
		else
		{
			// It's not a shift character
			char c=alphabet[character];
			if(c) text+=c;
		}

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
			case 2:
			case 3:
			{
				reader.moveNext();
				
				int bank=character;
				int offset=reader.getCurrent();
				int index=(32*(bank-1))+offset;
				text+=getAbbreviation(index);

				break;
			}				

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