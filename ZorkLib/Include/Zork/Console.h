#pragma once

#include <string>
#include <iostream>

#include <Zork\CoreTypes.h>

namespace zork
{

/** Defines console interaction with the user */
class Console
{
public:
	virtual ~Console()
	{
	}

	/** Prints text */
	virtual void print(const std::string &text) const = 0;
	
	/** Prints a number */
	virtual void print(SWord number) const = 0;
	
	/** Prints a newline */
	virtual void newline() const = 0;
	
	/** Reads text */
	virtual std::string read(size_t numberOfCharacters) const = 0;
};

class TextConsole : public Console
{
public:
	void print(const std::string &text) const override
	{
		std::cout << text;
	}

	void print(SWord number) const override
	{
		std::cout << number;
	}

	void newline() const override
	{
		std::cout << std::endl;
	}

	std::string read(size_t) const override
	{
		std::string text;
		std::getline(std::cin, text);

		return text;
	}
};

} // end of namespace