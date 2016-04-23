#include <Zork\Instructions.h>

namespace zork
{

std::ostream &operator<<(std::ostream &stream, OpcodeForm opcodeForm)
{
	switch(opcodeForm)
	{
		case OpcodeForm::Long:		return stream << "Long";
		case OpcodeForm::Short:		return stream << "Short";
		case OpcodeForm::Variable:	return stream << "Variable";
		case OpcodeForm::Extended:	return stream << "Extended";
		default:					return stream << "<unknown>";
	}
}

std::ostream &operator<<(std::ostream &stream, OperandCount operandCount)
{
	switch(operandCount)
	{
		case OperandCount::OP0:			return stream << "OP0";
		case OperandCount::OP1:			return stream << "OP1";
		case OperandCount::OP2:			return stream << "OP2";
		case OperandCount::Variable:	return stream << "Variable";
		default:						return stream << "<unknown>";
	}
}

std::ostream &operator<<(std::ostream &stream, OperandType operandType)
{
	switch(operandType)
	{
		case OperandType::Large:		return stream << "Large";
		case OperandType::Small:		return stream << "Small";
		case OperandType::Variable:		return stream << "Variable";
		case OperandType::Omitted:		return stream << "Omitted";
		default:						return stream << "<unknown>";
	}
}
std::ostream &operator<<(std::ostream &stream, OpcodeDetails opcodeDetails)
{
	auto flags = stream.flags();

	stream	<< "Address = " << std::hex << opcodeDetails.getBaseAddress() << std::dec
			<< ", OperandCount = " << opcodeDetails.getOperandCount()
			<< ", OperandForm = " << opcodeDetails.getOpcodeForm()
			<< ", DecodedOpcode = " << (int)opcodeDetails.getDecodedOpcode()
			<< ", EncodedOpcode = " << (int)opcodeDetails.getEncodedOpcode();

	stream.flags(flags);
	return stream;
}

} // end of namespace