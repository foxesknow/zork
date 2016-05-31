#include <algorithm>
#include <iostream>

#include <Zork\Story.h>

namespace zork
{

void Story::run()
{
	if(m_Version < 6)
	{
		setPC(m_AddressSpace.readWord(0x6));
	}
	else
	{
		auto address = m_AddressSpace.readWord(0x6);
		setPC(expandPackedRoutineAddress(address));
	}

	allocateNewFrame(0, 0, 0, DiscardResultsVariable);

	m_KeepRunning = true;

	while(m_KeepRunning)
	{
		if(m_PC == 0xcce)
		{
			//std::cout << "Break" << std::endl;
		}

		std::cout << std::hex << m_PC << std::dec << std::endl;

		executeNextInstruction();
		//std::cout << std::hex << m_PC << std::endl;
	}
}

void Story::setPC(Address address)
{
	m_PC = address;
}

void Story::executeNextInstruction()
{
	const auto opcodeDetails=decode();

	OperandType type1 = OperandType::Omitted;
	OperandType type2 = OperandType::Omitted;
	OperandType type3 = OperandType::Omitted;
	OperandType type4 = OperandType::Omitted;

	auto opcodeForm = opcodeDetails.getOpcodeForm();

	switch(opcodeForm)
	{
		case OpcodeForm::Short:
		{
			Byte type = (opcodeDetails.getEncodedOpcode() & 48) >> 4;
			type1 = ToOperandType(type);
			break;
		}

		case OpcodeForm::Long:
			type1 = (opcodeDetails.getEncodedOpcode() & 64) ? OperandType::Variable : OperandType::Small;
			type2 = (opcodeDetails.getEncodedOpcode() & 32) ? OperandType::Variable : OperandType::Small;
			break;

		case OpcodeForm::Variable:
		case OpcodeForm::Extended:
			Byte packed = readNextByte();
			unpackOperandTypes(packed, type1, type2, type3, type4);
			break;
	}

	auto operandCount = opcodeDetails.getOperandCount();

	if(m_Version >= 5 && opcodeDetails.getEncodedOpcode() == 190)
	{
		executeEXT(opcodeDetails, type1, type2, type3, type4);
	}
	else
	{
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
				executeOP1(opcodeDetails ,type1);
				break;

			case OperandCount::OP2:
				executeOP2(opcodeDetails, type1, type2, type3, type4);
				break;

			case OperandCount::Variable:
				executeVAR(opcodeDetails, type1, type2, type3, type4);
				break;
		}
	}
}

void Story::unpackOperandTypes(Byte encodedValue, OperandType &type1, OperandType &type2, OperandType &type3, OperandType &type4) const
{
	type4=ToOperandType(encodedValue & 3); encodedValue >>= 2;
	type3=ToOperandType(encodedValue & 3); encodedValue >>= 2;
	type2=ToOperandType(encodedValue & 3); encodedValue >>= 2;
	type1=ToOperandType(encodedValue & 3);
}

void Story::storeVariable(Byte variableID, Word value)
{
	if(variableID == TopOfStack)
	{
		// Variable 0 means top of stack
		push(value);
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

void Story::storeVariableInPlace(Byte variableID, Word value)
{
	if(variableID == 0)
	{
		// We write over the top of the stack
		pop();
		push(value);
	}
	else
	{
		storeVariable(variableID, value);
	}
}

Word Story::loadVariable(Byte variableID)
{
	if(variableID == TopOfStack)
	{
		// Variable 0 means top of stack
		return pop();
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

Word Story::loadVariableInPlace(Byte variableID)
{
	if(variableID == 0)
	{
		// We return the peek of the stack
		return peek();
	}
	else
	{
		return loadVariable(variableID);
	}
}

OpcodeDetails Story::decode()
{
	auto baseAddress = m_PC;

	const Byte value = readNextByte();
	auto opcodeForm=ToOpcodeForm(value);

	if(opcodeForm == OpcodeForm::Short)
	{
		auto operandCount = ((value & 48) == 48 ? OperandCount::OP0 : OperandCount::OP1);

		// The opcode is in the bottom 4 bits
		return OpcodeDetails(baseAddress, value, value & 15, opcodeForm, operandCount);
	}

	if(opcodeForm == OpcodeForm::Long)
	{
		auto operandCount=OperandCount::OP2;
		
		// The opcode is in the bottom 5 bits
		return OpcodeDetails(baseAddress,value,value & 31, opcodeForm, operandCount);
	}

	if(opcodeForm == OpcodeForm::Variable)
	{
		auto operandCount = ((value & 32) ? OperandCount::Variable : OperandCount::OP2);

		// The opcode is in the bottom 5 bits
		return OpcodeDetails(baseAddress, value, value & 31, opcodeForm, operandCount);
	}

	if(opcodeForm == OpcodeForm::Extended)
	{
		auto operandCount = OperandCount::Variable;

		// The next byte has the opcode
		Byte opcode = readNextByte();

		return OpcodeDetails(baseAddress, value, opcode, opcodeForm, operandCount);
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
		offset = ((offset << 8 ) | b2);

		// We've got a signed 14 bit number. We need to sign extend it to 16 bits if bit 14 is set
		if(offset & 8192)
		{
			offset |= 49152;
		}
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
		setPC(newPC);
	}
}

StackFrame Story::allocateNewFrame(Address returnAddress, size_t numberOfParameters, unsigned int numberOfLocals, Word returnVariable)
{
	auto stackFrame = m_StackSpace.allocateNewFrame(returnAddress, numberOfParameters, numberOfLocals, returnVariable);
	m_Frames.push(stackFrame);

	return stackFrame;
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

	// Point to the new routine
	setPC(normalizedAddress);

	auto numberOfLocals = readNextByte();
	auto stackFrame = allocateNewFrame(returnAddress, arguments.size(), numberOfLocals, returnVariable);

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
	setPC(returnFrame.getReturnAddress());

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

void Story::push(Word value)
{
	m_StackSpace.push(value);

#ifdef _DEBUG
	if((m_StackSpace.getSP() - m_Frames.top().getBase()) < m_Frames.top().getNumberOfLocals()) 
	{
		panic("oops");
	}
#endif
}

Word Story::pop()
{
	auto value = m_StackSpace.pop();

#ifdef _DEBUG
	if((m_StackSpace.getSP() - m_Frames.top().getBase()) < m_Frames.top().getNumberOfLocals()) 
	{
		panic("oops");
	}
#endif

	return value;
}

Word Story::peek()
{
	auto value = m_StackSpace.peek();

#ifdef _DEBUG
	if((m_StackSpace.getSP() - m_Frames.top().getBase()) < m_Frames.top().getNumberOfLocals()) 
	{
		panic("oops");
	}
#endif

	return value;
}

} // end of namespace