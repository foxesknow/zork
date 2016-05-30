#include <Zork\Story.h>

namespace zork
{

void Story::executeOP0(const OpcodeDetails &opcodeDetails)
{
	const auto opcode = static_cast<OP0_Opcodes>(opcodeDetails.getDecodedOpcode());

	switch(opcode)
	{
		case OP0_Opcodes::OP176: 
			handle_rtrue();
			break;

		case OP0_Opcodes::OP177:
			handle_rtrue();
			break;

		case OP0_Opcodes::OP178:
		case OP0_Opcodes::OP179:
			handle_print(opcode);
			break;

		case OP0_Opcodes::OP180: // nop
			// Do nothing!
			break;

		case OP0_Opcodes::OP184:
			handle_ret_popped();
			break;

		case OP0_Opcodes::OP185: 
			handle_pop_catch();
			break;

		case OP0_Opcodes::OP187:
			handle_newline();
			break;

		case OP0_Opcodes::OP189: 
			handle_verify();
			break;

		case OP0_Opcodes::OP190:
			// We shouldn't get here as executeNextInstruction should have handled this
			panic("op0_op190 should have been handled by executeNextInstruction");
			break;

		case OP0_Opcodes::OP191:
			handle_piracy();
			break;

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}

void Story::handle_rtrue()
{
	// rtrue
	returnFromCall(1);
}

void Story::handle_rfalse()
{
	// rtrue
	returnFromCall(0);
}

void Story::handle_print(const OP0_Opcodes &opcode)
{
	AddressSpaceZsciiReader reader(m_AddressSpace, m_PC);
	auto string = readString(reader);
	m_Console->print(string);

	if(opcode == OP0_Opcodes::OP178)
	{
		setPC(increaseWordAddress(reader.getAddress(), 1)); // +1 as the address is the end of the string
	}
	else if(opcode == OP0_Opcodes::OP179)
	{
		returnFromCall(1);
	}
	else
	{
		panic("unexpected opcode");
	}
}

void Story::handle_ret_popped()
{
	// ret_popped
	// Return the item on the top of the stack
	auto returnValue = loadVariable(0);
	returnFromCall(returnValue);
}

void Story::handle_pop_catch()
{
	// (v1) pop, (v5-v6) catch

	if(m_Version < 4) // pop
	{
		// It's a pop. Just throw away the value
		pop();
	}
	else if(m_Version == 5 || m_Version == 6) // catch -> (result)
	{
		auto variableID = readVariableID();
		auto frameID = static_cast<Word>(m_Frames.size());
		storeVariable(variableID, frameID);
	}
	else
	{
		panic("op0_op185 not supported in this version");
	}
}

void Story::handle_newline()
{
	m_Console->newline();
}

void Story::handle_verify()
{
	// verify ?(label)
	if(m_Version < 3) panic("op0_op189 requires v3 or more");

	Address nextByte = 0x40;
	auto fileSize = getFileSize();
	Word calculatedChecksum = 0;

	for(int i = 0; i < fileSize; i++)
	{
		// NOTE: This is intended to overflow
		calculatedChecksum += m_AddressSpace.readByte(nextByte++);
	}

	Word checksumFromHeader = m_AddressSpace.readWord(0x1c);
	auto outcome = ( calculatedChecksum == checksumFromHeader);

	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_piracy()
{
	// piracy ?(label)
	if(m_Version >= 5)
	{
		bool genuine = true;
		auto branchDetails = readBranchDetails();
		if(branchDetails.shouldBranch(genuine)) applyBranch(branchDetails);
	}
	else
	{
		panic("op0_op191 not supported in this version");
	}
}

} // end of namespace