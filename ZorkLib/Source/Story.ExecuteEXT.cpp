#include <cstdlib>
#include <Zork\Story.h>

namespace zork
{

void Story::executeEXT(const OpcodeDetails &opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
	const auto opcode = static_cast<EXT_Opcodes>(opcodeDetails.getDecodedOpcode());

	switch(opcode)
	{
		case EXT_Opcodes::OP3:
			handle_art_shift(type1, type2);
			break;

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}

void Story::handle_art_shift(OperandType type1, OperandType type2)
{
	auto number = read(type1);
	auto places = AsSignedWord(read(type2));

	Word result = 0;
	if(places < 0)
	{
		// It's a shift to the right
		result = AsSignedWord(number) >> std::abs(places);
	}
	else
	{
		// A shift to the left
		result = AsSignedWord(number) << places;
	}

	auto variableID = readVariableID();
	storeVariable(variableID, result);
}

} // end of namespace