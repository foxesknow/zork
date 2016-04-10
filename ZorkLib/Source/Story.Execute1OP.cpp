#include <Zork\Story.h>

namespace zork
{

void Story::executeOP1(OpcodeDetails opcodeDetails, OperandType type1)
{
	const auto opcode = static_cast<OP1_Opcodes>(opcodeDetails.getDecodedOpcode());
	const auto a = read(type1);

	switch(opcode)
	{
		case OP1_Opcodes::OP128: // jz a ?(label)
		{
			// Jump if a is 0
			bool outcome = (a == 0);
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
			break;
		}

		default:
		{
			ThrowNotImplemented();
			break;
		}
	}
}

} // end of namespace