#pragma once

#include <string>
#include <iostream>

#include <Zork\CoreTypes.h>

namespace zork
{

class Console
{
public:
	virtual ~Console()
	{
	}

	virtual void print(const std::string &text) const = 0;
	virtual void print(SWord number) const = 0;
	virtual void newline() const = 0;
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
};

} // end of namespace