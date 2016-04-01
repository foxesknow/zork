#include <Zork\Story.h>

namespace zork
{

void Story::executeNextInstruction()
{
	OpcodeForm opcodeForm;
	OperandCount operandCount;

	const auto opcodeDetails=decode(opcodeForm,operandCount);

	OperandType type1=OperandType::Omitted;
	OperandType type2=OperandType::Omitted;
	OperandType type3=OperandType::Omitted;
	OperandType type4=OperandType::Omitted;

	switch(opcodeForm)
	{
		case OpcodeForm::Short:
		{
			Byte type=(opcodeDetails.getEncodedOpcode() & 48) >> 4;
			type1=ToOperandType(type);
			break;
		}

		case OpcodeForm::Long:
			type1=(opcodeDetails.getEncodedOpcode() & 32) ? OperandType::Variable : OperandType::Small;
			type2=(opcodeDetails.getEncodedOpcode() & 32) ? OperandType::Variable : OperandType::Small;
			break;

		case OpcodeForm::Variable:
		case OpcodeForm::Extended:
			Byte packed=readNextByte();
			type4=ToOperandType(packed & 3); packed>>=2;
			type3=ToOperandType(packed & 3); packed>>=2;
			type2=ToOperandType(packed & 3); packed>>=2;
			type1=ToOperandType(packed & 3);
			break;
	}

	switch(operandCount)
	{
		case OperandCount::OP0:
			executeOP0(opcodeDetails);
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

void Story::executeOP0(OpcodeDetails opcodeDetails)
{
}

void Story::executeOP1(OpcodeDetails opcodeDetails, OperandType type1)
{
}

void Story::executeOP2(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2)
{
}

void Story::executeVAR(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
}


void Story::storeVariable(Byte variableID, Word value)
{
	// TODO
}

Word Story::loadVariable(Byte variableID)
{
	// TODO
	return 0;
}

OpcodeDetails Story::decode(OpcodeForm &opcodeForm, OperandCount &operandCount)
{
	auto baseAddress=m_PC;
	const Byte value=readNextByte();

	opcodeForm=ToOpcodeForm(value);

	if(opcodeForm==OpcodeForm::Short)
	{
		operandCount=((value & 48)==48 ? OperandCount::OP0 : OperandCount::OP1);

		// The opcode is in the bottom 4 bits
		return OpcodeDetails(baseAddress,value,value & 15);
	}

	if(opcodeForm==OpcodeForm::Long)
	{
		operandCount=OperandCount::OP2;
		
		// The opcode is in the bottom 5 bits
		return OpcodeDetails(baseAddress,value,value & 31);
	}

	if(opcodeForm==OpcodeForm::Variable)
	{
		operandCount=((value & 32) ? OperandCount::Variable : OperandCount::OP2);

		// The opcode is in the bottom 5 bits
		return OpcodeDetails(baseAddress,value,value & 31);
	}

	if(opcodeForm==OpcodeForm::Extended)
	{
		operandCount=OperandCount::Variable;

		// The next byte has the opcode
		Byte opcode=readNextByte();

		return OpcodeDetails(baseAddress,value,opcode);
	}

	throw Exception("unexpected opcode form");
}

} // end of namespace