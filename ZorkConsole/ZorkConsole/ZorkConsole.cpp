// ZorkConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <Zork\CoreTypes.h>
#include <Zork\AddressSpace.h>
#include <Zork\Story.h>
#include <Zork\StackSpace.h>
#include <Zork\Instructions.h>

int main()
{
	using namespace zork;

    std::string filename="D:\\Git\\zork\\StoryFiles\\minizork.z3";
	//std::string filename="D:\\Git\\zork\\StoryFiles\\Zork\\zork2.z3";
	//std::string filename="D:\\Git\\zork\\StoryFiles\\moonmist\\moonmist.z5";

	AddressSpace addressSpace=zork::loadAddressSpaceFromFile(filename);

	Story story(std::move(addressSpace));

	//auto s=story.readString(0xb106);
	//std::cout << s << std::endl;

	return 0;
}

