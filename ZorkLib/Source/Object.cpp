#include <Zork\Object.h>

namespace zork
{

Word Object::getPropertLengthForAddress(AddressSpace &addressSpace, Address address, int version)
{
	/*
	 * This one's a bit odd as the address is the address of the property, not the size field before it...
	 * If you pass an address of 0 then the property size is 0
	 * Up to V3 the size is on the byte before the property
	 * After V3 the size field can be 1 or 2 bytes. However, the way the encoding works the size 
	 * can always be inferred from the byte before the property.
	 * If the high-bit is set then the bottom 6 bit contain the size
	 * If the high bit isn't set then if bit-6 is set the size is 2, otherwise it's 1
	 */
	if(address == 0) return 0;

	bool v3OrLess = versionThreeOrLess(version, true, false);
	auto b = addressSpace.readByte(address - 1);
	Word size = 0;

	if(v3OrLess)
	{
		size = (b >> 5) + 1;
	}
	else
	{
		if(b & 128)
		{
			size = (b & 63);
			if(size == 0) size = 64;
		}
		else
		{
			size = (b & 64 ? 2 : 1);
		}
	}

	return size;
}

std::tuple<Address,int,Word,bool> Object::getPropertyBlockInfo(Address address)const
{
	// TODO: Handle version 4 and above!

	// We'll return (StartOfData, SizeOfData, PropertyNumber, Terminator)

	// address points to the start of the size block

	if(m_V3OrLess)
	{
		// V1-3 property blocks are: [size byte][property bytes(between 1 and 8)]

		const Byte value = m_AddressSpace.readByte(address);

		// The size is the upper 3 bytes, plus 1!
		const int size = (value >> 5) + 1;

		// The property number is in the lower 5 bits
		const auto propertyNumber = value & 31;

		return std::make_tuple(address+1, size, propertyNumber, value == 0);
	}
	else
	{
		// V4+ property blocks are [size and number (1 or 2 bytes)[property bytes(between 1 and 64)]

		const Byte value=m_AddressSpace.readByte(address);

		const Word propertyNumber = value & 63;
		int size=0;

		// If bit 7 is set then there a 2nd size byte
		if(value & 128)
		{
			address++;
			auto encodedSize = m_AddressSpace.readByte(address);

			size = encodedSize & 63;
			if(size == 0) size = 64;
		}
		else
		{
			// There isn't an extra size field.
			// If bit 7 isn't set then bit 6 indicates if the length is 2 (set to 1), or 1 (set to 0)
			auto shortSizeMarker = value & 64;
			size = (shortSizeMarker ? 2 : 1);
		}

		return std::make_tuple(address+1, size, propertyNumber, value==0);
	}

}

PropertyBlock Object::getPropertyBlock(int index)const
{
	if(index<1) throw Exception("invalid property block index");

	auto base = getPropertyBlockBase();

	for(;;)
	{
		auto blockInfo = getPropertyBlockInfo(base);

		bool terminator = std::get<3>(blockInfo);
		if(terminator)
		{
			throw Exception("encountered block terminator");
		}

		// The property number is in the lower 5 bits
		const auto propertyNumber = std::get<2>(blockInfo);

		// The indicies are stored in descending numerical order, so we can check for an invalid block index
		if(index>propertyNumber) throw Exception("no such property block");

		const int size = std::get<1>(blockInfo);
		const Address address = std::get<0>(blockInfo);

		if(index == propertyNumber)
		{
			return PropertyBlock(address, size, propertyNumber);
		}
		
		base=address + size;
	}
}

std::vector<PropertyBlock> Object::getAllPropertyBlocks()const
{
	std::vector<PropertyBlock> properties;

	auto base = getPropertyBlockBase();

	for(;;)
	{
		auto blockInfo = getPropertyBlockInfo(base);

		bool terminator = std::get<3>(blockInfo);
		if(terminator) break;

		// The property number is in the lower 5 bits
		const auto propertyNumber = std::get<2>(blockInfo);
		const int size = std::get<1>(blockInfo);
		const Address address = std::get<0>(blockInfo);

		properties.push_back(PropertyBlock(address, size, propertyNumber));
		
		base=address + size;
	}

	return properties;
}

} // end of namespace