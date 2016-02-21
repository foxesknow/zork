// ZorkConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Zork\CoreTypes.h>
#include <Zork\AddressSpace.h>

int main()
{
    const char *filename="D:\\Git\\zork\\StoryFiles\\minizork.z3";

	zork::AddressSpace addressSpace=zork::loadAddressSpaceFromFile(filename);

	return 0;
}

