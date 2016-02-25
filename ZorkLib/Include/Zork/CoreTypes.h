#pragma once

#include <cstdint>
#include <string>

namespace zork
{

typedef uint8_t Byte;
typedef uint16_t Word;
typedef uint32_t Address;

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

inline Address increaseWordAddress(Address address, int amount)
{
	auto offset=sizeof(Word)*amount;
	return static_cast<Address>(address+offset);
}


} // End of namespace