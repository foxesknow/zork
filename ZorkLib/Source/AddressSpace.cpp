#include <iostream>
#include <fstream>

#include <Zork\AddressSpace.h>

namespace zork
{

AddressSpace loadAddressSpaceFromFile(const std::string &filename)
{
	using namespace std;
	
	std::ifstream file;
	file.open(filename,ios::in|ios::binary);

	// Work out the size of the file
	file.seekg(0,ios::end);
	size_t size=file.tellg();

	file.seekg(0,ios::beg);

	AddressSpace addressSpace(size);

	char *base=reinterpret_cast<char*>(addressSpace.base());
	file.read(base,size);

	return addressSpace;
}

} // end of namespace