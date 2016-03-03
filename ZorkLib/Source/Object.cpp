#include <Zork\Object.h>

namespace zork
{

std::tuple<Address,int,int,bool> Object::getPropertyBlockInfo(Address address)const
{
	// TODO: Handle version 4 and above!

	// We'll return (StartOfData, SizeOfData, PropertyNumber, Terminator)

	// address points to the start of the size block

	const Byte value=m_AddressSpace.readByte(address);

	// The size is the upper 3 bytes, plus 1!
	const Byte size=(value>>5)+1;

	// The property number is in the lower 5 bits
	const auto propertyNumber=value&0x1f;

	return std::make_tuple(address+1,size,propertyNumber,value==0);

}

PropertyBlock Object::getPropertyBlock(int index)const
{
	if(index<1) throw Exception("invalid property block index");

	auto base=getPropertyBlockBase();

	for(;;)
	{
		auto blockInfo=getPropertyBlockInfo(base);

		bool terminator=std::get<3>(blockInfo);
		if(terminator)
		{
			throw Exception("encountered block terminator");
		}

		// The property number is in the lower 5 bits
		const auto propertyNumber=std::get<2>(blockInfo);

		// The indicies are stored in descending numerical order, so we can check for an invalid block index
		if(index>propertyNumber) throw Exception("no such property block");

		const int size=std::get<1>(blockInfo);
		const Address address=std::get<0>(blockInfo);

		if(index==propertyNumber)
		{
			return PropertyBlock(address,size,propertyNumber);
		}
		
		base=address+size;
	}
}

std::vector<PropertyBlock> Object::getAllPropertyBlocks()const
{
	std::vector<PropertyBlock> properties;

	auto base=getPropertyBlockBase();

	for(;;)
	{
		auto blockInfo=getPropertyBlockInfo(base);

		bool terminator=std::get<3>(blockInfo);
		if(terminator) break;

		// The property number is in the lower 5 bits
		const auto propertyNumber=std::get<2>(blockInfo);
		const int size=std::get<1>(blockInfo);
		const Address address=std::get<0>(blockInfo);

		properties.push_back(PropertyBlock(address,size,propertyNumber));
		
		base=address+size;
	}

	return properties;
}

} // end of namespace