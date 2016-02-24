// ZorkConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Zork\CoreTypes.h>
#include <Zork\AddressSpace.h>
#include <Zork\Story.h>

int main()
{
    std::string filename="D:\\Git\\zork\\StoryFiles\\minizork.z3";

	zork::AddressSpace addressSpace=zork::loadAddressSpaceFromFile(filename);

	zork::Story story(std::move(addressSpace));
	story.buildAbbreviationCache();

	return 0;
}

