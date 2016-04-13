#include <algorithm>
#include <Zork\Story.h>

namespace zork
{

void Story::executeNextInstruction()
{
	OpcodeForm opcodeForm;
	OperandCount operandCount;

	const auto opcodeDetails=decode(opcodeForm,operandCount);

	OperandType type1 = OperandType::Omitted;
	OperandType type2 = OperandType::Omitted;
	OperandType type3 = OperandType::Omitted;
	OperandType type4 = OperandType::Omitted;

	switch(opcodeForm)
	{
		case OpcodeForm::Short:
		{
			Byte type = (opcodeDetails.getEncodedOpcode() & 48) >> 4;
			type1 = ToOperandType(type);
			break;
		}

		case OpcodeForm::Long:
			type1 = (opcodeDetails.getEncodedOpcode() & 32) ? OperandType::Variable : OperandType::Small;
			type2 = (opcodeDetails.getEncodedOpcode() & 32) ? OperandType::Variable : OperandType::Small;
			break;

		case OpcodeForm::Variable:
		case OpcodeForm::Extended:
			Byte packed = readNextByte();
			type4=ToOperandType(packed & 3); packed >>= 2;
			type3=ToOperandType(packed & 3); packed >>= 2;
			type2=ToOperandType(packed & 3); packed >>= 2;
			type1=ToOperandType(packed & 3);
			break;
	}

	switch(operandCount)
	{
		case OperandCount::OP0:
			if(opcodeForm == OpcodeForm::Extended)
			{
				executeEXT(opcodeDetails, type1, type2, type3, type4);
			}
			else
			{
				executeOP0(opcodeDetails);
			}
			break;

		case OperandCount::OP1:
			executeOP1(opcodeDetails,type1);
			break;

		case OperandCount::OP2:
			executeOP2(opcodeDetails,type1,type2);
			break;

		case OperandCount::Variable:
			executeVAR(opcodeDetails,type1,type2,type3,type4);
			break;
	}
}

void Story::executeVAR(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
}

void Story::executeEXT(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
}

void Story::storeVariable(Byte variableID, Word value)
{
	if(variableID == TopOfStack)
	{
		// Variable 0 means top of stack
		m_StackSpace.push(value);
	}
	else if(variableID >= BeginLocal && variableID <= EndLocal)
	{
		// These are local variables
		const auto &activeFrame = m_Frames.top();
		m_StackSpace.setLocal(activeFrame, variableID, value);
	}
	else
	{
		// It's a global
		auto address = increaseWordAddress(m_GlobalVariablesTable, variableID - BeginGlobal);
		m_AddressSpace.writeWord(address, value);
	}
}

Word Story::loadVariable(Byte variableID)
{
	if(variableID == TopOfStack)
	{
		// Variable 0 means top of stack
		return m_StackSpace.pop();
	}
	else if(variableID >= BeginLocal && variableID <= EndLocal)
	{
		// These are local variables
		const auto &activeFrame = m_Frames.top();
		return m_StackSpace.getLocal(activeFrame, variableID);
	}
	else
	{
		// It's a global
		auto address = increaseWordAddress(m_GlobalVariablesTable, variableID - BeginGlobal);
		return m_AddressSpace.readWord(address);
	}
}

OpcodeDetails Story::decode(OpcodeForm &opcodeForm, OperandCount &operandCount)
{
	auto baseAddress = m_PC;
	const Byte value = readNextByte();

	opcodeForm=ToOpcodeForm(value);

	if(opcodeForm == OpcodeForm::Short)
	{
		operandCount = ((value & 48) == 48 ? OperandCount::OP0 : OperandCount::OP1);

		// The opcode is in the bottom 4 bits
		return OpcodeDetails(baseAddress, value, value & 15);
	}

	if(opcodeForm == OpcodeForm::Long)
	{
		operandCount=OperandCount::OP2;
		
		// The opcode is in the bottom 5 bits
		return OpcodeDetails(baseAddress,value,value & 31);
	}

	if(opcodeForm == OpcodeForm::Variable)
	{
		operandCount = ((value & 32) ? OperandCount::Variable : OperandCount::OP2);

		// The opcode is in the bottom 5 bits
		return OpcodeDetails(baseAddress, value, value & 31);
	}

	if(opcodeForm == OpcodeForm::Extended)
	{
		operandCount = OperandCount::Variable;

		// The next byte has the opcode
		Byte opcode = readNextByte();

		return OpcodeDetails(baseAddress, value, opcode);
	}

	throw Exception("unexpected opcode form");
}

Word Story::read(OperandType operandType)
{
	switch(operandType)
	{
		case OperandType::Large:
			return readNextWord();

		case OperandType::Small:
			return readNextByte();

		case OperandType::Variable:
		{
			auto variableID = readNextByte();
			return loadVariable(variableID);
		}

		case OperandType::Omitted:
		{
			throw Exception("Omitted unexpected");
		}

		default:
		{
			throw Exception("unexpected operand type");
		}
	}
}

BranchDetails Story::readBranchDetails()
{
	Byte b1 = readNextByte();

	// If bit 7 is 0 the condition must be false, if 1 then the condition must be true
	bool comparisonValue = (b1 & 128) ? true : false;

	Word offset = (b1 & 63);

	// If bit 6 is set then the branch is 1 byte, otherwise it's 2
	if((b1 & 64) == 0)
	{
		auto b2 = readNextByte();
		offset = (offset <<8 ) | b2;
	}

	return BranchDetails(AsSignedWord(offset), comparisonValue);
}

void Story::applyBranch(const BranchDetails &branchDetails)
{
	applyBranch(branchDetails.getOffset());
}

void Story::applyBranch(SWord offset)
{
	if(offset == 0)
	{
		// Return false
		returnFromCall(0);
	}
	else if(offset == 1)
	{
		// Return true
		returnFromCall(1);
	}
	else
	{
		Address newPC = m_PC + (offset - 2);
		m_PC = newPC;
	}
}

void Story::callRoutine(Address routineAddress, Word returnVariable, const std::vector<Word> &arguments)
{
	// NOTE: special case! A call to address 0 means return false!
	if(routineAddress == 0)
	{
		returnFromCall(0);
		return; // NOTE: Early return
	}

	Address returnAddress = m_PC;

	auto normalizedAddress = expandPackedRoutineAddress(routineAddress);
	auto numberOfLocals = m_AddressSpace.readByte(normalizedAddress);

	auto stackFrame = m_StackSpace.allocateNewFrame(returnAddress, numberOfLocals, returnVariable);
	m_Frames.push(stackFrame);

	// Point to the new routine
	m_PC = normalizedAddress;

	// The local default values are stored next
	if(m_Version <= 4)
	{
		// The defaults are next
		for(Byte i = 0; i < numberOfLocals; i++)
		{
			auto value = readNextWord();
			storeVariable(i + 1, value);
		}
	}
	else
	{
		// They all default to 0
		for(Byte i = 0; i < numberOfLocals; i++)
		{
			storeVariable(i + 1, 0);
		}
	}

	// Now layer the arguments on top
	auto argumentsToCopy = std::min(numberOfLocals, static_cast<Byte>(arguments.size()));
	for(Byte i = 0; i < argumentsToCopy; i++)
	{
		storeVariable(i + 1, arguments[i]);
	}

}

void Story::returnFromCall(Word result)
{
	// First up, put the stack and PC back to what they should be
	auto returnFrame = m_Frames.top();
	m_Frames.pop();

	m_StackSpace.revertToFrame(returnFrame);
	m_PC = returnFrame.getReturnAddress();

	// The result goes back into a variable...
	auto resultVariableID = returnFrame.getResultVariable();
	
	if(resultVariableID != DiscardResultsVariable)
	{
		storeVariable(static_cast<Byte>(resultVariableID), result);
	}
}

void Story::unwindToFrame(Word frameID)
{
	if(frameID > m_Frames.size())
	{
		panic("cannot unwind to a frame which does not exist");
	}

	while(m_Frames.size() != frameID)
	{
		m_Frames.pop();
	}
}


} // end of namespace