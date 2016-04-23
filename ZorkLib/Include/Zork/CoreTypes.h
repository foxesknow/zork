#pragma once

#include <cstdint>
#include <string>
#include <limits>
#include <iostream>

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

/** Indicates that the results of a routine should be discarded */
const Word DiscardResultsVariable = std::numeric_limits<Word>::max();

/** Base class for zork exceptions */
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

inline std::ostream &operator<<(std::ostream &stream, const Exception &e)
{
	return stream << e.reason();
}

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

inline void panic(const std::string &message)
{
	throw Exception(message);
}

class DictionaryEntry
{
private:
	Address m_Address;
	std::string m_String;

public:
	DictionaryEntry(Address address, const std::string &string) : m_Address(address), m_String(string)
	{
	}

	Address getAddress() const
	{
		return m_Address;
	}

	const std::string &getString() const
	{
		return m_String;
	}
};

inline bool operator==(const DictionaryEntry &lhs, const DictionaryEntry &rhs)
{
	return lhs.getString() == rhs.getString();
}

inline bool operator<(const DictionaryEntry &lhs, const DictionaryEntry &rhs)
{
	return lhs.getString() < rhs.getString();
}


} // End of namespace