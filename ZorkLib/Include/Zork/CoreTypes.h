#pragma once

#include <cstdint>
#include <string>

namespace zork
{

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t address_t;

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

} // End of namespace