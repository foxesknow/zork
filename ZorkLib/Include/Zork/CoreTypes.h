#pragma once

#include <cstdint>
#include <string>

namespace zork
{

typedef uint8_t Byte;
typedef uint16_t Word;
typedef uint32_t Address;

// There time when we might need to generate a zscii null
// This is an end of string marker (the first one) followed by 3 5s
const Word ZsciiNull=0b1001010010100101;

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

constexpr Address increaseWordAddress(Address address, int amount)
{
	return static_cast<Address>(address+(sizeof(Word)*amount));
}

template<class T>
constexpr T versionThreeOrLess(int version, T ifTrue, T ifFalse)
{
	return version<=3 ? ifTrue : ifFalse;
}


} // End of namespace