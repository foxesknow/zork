#pragma once

#include <cstdint>
#include <string>

namespace zork
{

typedef uint8_t Byte;
typedef uint16_t Word;
typedef uint32_t Address;
typedef int16_t SWord;

constexpr SWord AsSignedWord(Word word)
{
	return static_cast<SWord>(word);
}

constexpr Word AsWord(SWord sword)
{
	return static_cast<Word>(sword);
}

const Byte TopOfStack = 0;
const Byte BeginLocal = 1;
const Byte EndLocal = 15;
const Byte BeginGlobal = 16;
const Byte EndGlobal = 255;

// There time when we might need to generate a zscii null
// This is an end of string marker (the first one) followed by 3 5s
const Word ZsciiNull = 0b1001010010100101;

class Exception
{
private:
	std::string m_Reason;

public:
	Exception()
	{
	}

	Exception(const std::string &reason) : m_Reason(reason)
	{
	}

	const std::string &reason()const
	{
		return m_Reason;
	}
};

/** Increases an address by the specified number of words */
constexpr Address increaseWordAddress(Address address, int amount)
{
	return static_cast<Address>(address + (sizeof(Word) * amount));
}

template<class T>
constexpr T versionThreeOrLess(int version, T ifTrue, T ifFalse)
{
	return version <= 3 ? ifTrue : ifFalse;
}


} // End of namespace