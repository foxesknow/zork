#include <cstdlib>
#include <Zork\Story.h>

namespace zork
{

void Story::executeVAR(const OpcodeDetails &opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
	const auto opcode = static_cast<VAR_Opcodes>(opcodeDetails.getDecodedOpcode());

	switch(opcode)
	{
		case VAR_Opcodes::OP224:
			handle_call(type1, type2, type3, type4); 
			break;

		case VAR_Opcodes::OP225:
			handle_storew(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP226:
			handle_storeb(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP227: 
			handle_put_prop(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP228:
			handle_sread(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP229:
			handle_print_char(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP230:
			handle_print_num(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP231: 
			handle_random(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP232: 
			handle_push(type1, type2, type3, type4);
			break;

		case VAR_Opcodes::OP233:
			handle_pull(type1, type2, type3, type4);
			break;

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}

void Story::handle_call(OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
	// call routine 1..3 args -> (result)
	const Word address = (IsPresent(type1) ? read(type1) : 0);

	auto arguments = createArguments({});

	// After the address are the potential arguments
	if(IsPresent(type2)) arguments.push_back(read(type2));
	if(IsPresent(type3)) arguments.push_back(read(type3));
	if(IsPresent(type4)) arguments.push_back(read(type4));

	auto variableID = readVariableID();

	callRoutine(address, variableID, arguments);
}

void Story::handle_storew(OperandType type1, OperandType type2, OperandType type3, OperandType)
{
	// storew array word-index value
	Address baseAddress = read(type1);
	int index = read(type2);
	auto value = read(type3);

	auto dataLocation = increaseWordAddress(baseAddress, index);
	m_AddressSpace.writeWord(dataLocation, value);
}

void Story::handle_storeb(OperandType type1, OperandType type2, OperandType type3, OperandType)
{
	// storeb array byte-index value
	Address baseAddress = read(type1);
	int index = read(type2);
	auto value = read(type3);

	auto dataLocation = baseAddress + index;
	m_AddressSpace.writeByte(dataLocation, static_cast<Byte>(value));

}

void Story::handle_put_prop(OperandType type1, OperandType type2, OperandType type3, OperandType)
{
	// put_prop object property value
	auto objectID = read(type1);
	auto propertyID = read(type2);
	auto value = read(type3);
	
	auto object = getObject(objectID);
	auto block = object.getPropertyBlock(propertyID);

	if(block.getSize() == 1)
	{
		m_AddressSpace.writeByte(block.getAddress(), static_cast<Byte>(value & 0xff));
	}
	else if(block.getSize() == 2)
	{
		m_AddressSpace.writeWord(block.getAddress(), value);
	}
	else
	{
		panic("cannot write value > 2 bytes long to a property block");
	}
}

void Story::handle_sread(OperandType type1, OperandType type2, OperandType, OperandType)
{
	Address textAddress = read(type1);
	Address parseAddress = read(type2);

	size_t maximumInputAllowed = m_AddressSpace.readByte(textAddress);
	auto enteredText = m_Console->read(maximumInputAllowed);

	auto textToParse = toLowerCase(enteredText);

	auto charsToTake = std::min(maximumInputAllowed, textToParse.size());
	textToParse.resize(charsToTake);

	// Write it to the text buffer
	textAddress++; // To move past the size marker
	for(auto c : textToParse)
	{
		m_AddressSpace.writeByte(textAddress, static_cast<Byte>(c));
		textAddress++;
	}

	// The text buffer needs a null
	m_AddressSpace.writeByte(textAddress, 0);

	auto tokens=tokenize(textToParse);
	size_t maximumTokensAllowed = m_AddressSpace.readByte(parseAddress);
	auto tokensToTake = std::min(maximumTokensAllowed, tokens.size());

	// Now write the parse blocks
	parseAddress++; // Move past the size marker

	// The first byte holds the number of tokens
	m_AddressSpace.writeByte(parseAddress++, static_cast<Byte>(tokensToTake));

	for(size_t i = 0; i < tokensToTake; i++)
	{
		const auto &token = tokens[i];

		// The address in the dictionary
		m_AddressSpace.writeWord(parseAddress, static_cast<Word>(token.getDictionaryAddress()));
		parseAddress = increaseWordAddress(parseAddress, 1);

		// The number of letters in the word
		m_AddressSpace.writeByte(parseAddress++, static_cast<Byte>(token.getText().size()));

		// The position of the word within the input string. 
		// It's +1 as the first char in the buffer is a size
		m_AddressSpace.writeByte(parseAddress++, token.getPosition()+1);
	}

	//throw Exception("TODO");
}

void Story::handle_print_char(OperandType type1, OperandType, OperandType, OperandType)
{
	// print_char zascii
	auto character = read(type1);

	std::string text(1,static_cast<char>(character));
	m_Console->print(text);
}

void Story::handle_print_num(OperandType type1, OperandType, OperandType, OperandType)
{
	// print_num value
	auto number = read(type1);
	m_Console->print(number);
}

void Story::handle_random(OperandType type1, OperandType, OperandType, OperandType)
{
	// random range -> result
	auto range = AsSignedWord(read(type1));
	auto variableID = readVariableID();

	if(range < 0)
	{
		// We need to seed
		std::srand(range);
	}
	else
	{
		// We need a number between 1 and range
		auto randomNumber = static_cast<Word>((std::rand() % range) + 1);
		storeVariable(variableID, randomNumber);
	}
}

void Story::handle_push(OperandType type1, OperandType, OperandType, OperandType)
{
	// push value
	auto value = read(type1);
	m_StackSpace.push(value);
}

void Story::handle_pull(OperandType type1, OperandType, OperandType, OperandType)
{
	// pull (variable)
	Byte variableID = static_cast<Byte>(read(type1));
	auto value = m_StackSpace.pop();
	storeVariableInPlace(variableID, value);
}

} // end of namespace