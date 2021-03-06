// ZorkConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <Zork\CoreTypes.h>
#include <Zork\AddressSpace.h>
#include <Zork\Story.h>
#include <Zork\StackSpace.h>
#include <Zork\Instructions.h>
#include <Zork\Console.h>

int main()
{
	using namespace zork;

    //std::string filename="f:\\Git\\zork\\StoryFiles\\minizork.z3";
	std::string filename="f:\\Git\\zork\\StoryFiles\\Zork\\zork1.z5";
	//std::string filename="f:\\Git\\zork\\StoryFiles\\czech\\czech.z5";

	AddressSpace addressSpace = zork::loadAddressSpaceFromFile(filename);
	auto console = std::make_shared<TextConsole>();

	Story story(std::move(addressSpace), console);

	try
	{
		story.run();
	}
	catch(const Exception &e)
	{
		std::cerr << e << std::endl;
	}

	return 0;
}

