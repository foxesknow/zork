#include <cstdlib>
#include <Zork\Story.h>

namespace zork
{

void Story::executeVAR(const OpcodeDetails &opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
	const auto opcode = static_cast<VAR_Opcodes>(opcodeDetails.getDecodedOpcode());

	const Word a = (IsPresent(type1) ? read(type1) : 0);
	const Word b = (IsPresent(type2) ? read(type2) : 0);
	const Word c = (IsPresent(type3) ? read(type3) : 0);
	const Word d = (IsPresent(type4) ? read(type4) : 0);

	switch(opcode)
	{
		case VAR_Opcodes::OP224: // call routine 1..3 args -> (result)
		{
			auto variableID = readVariableID();

			auto arguments = createArguments({});
			
			// a holds the address of the routine, the rest are potentially arguments
			if(IsPresent(type2)) arguments.push_back(b);
			if(IsPresent(type3)) arguments.push_back(c);
			if(IsPresent(type4)) arguments.push_back(d);

			callRoutine(a, variableID, arguments);
			break;
		}

		case VAR_Opcodes::OP225: // storew array word-index value
		{
			Address baseAddress = a;
			auto dataLocation = increaseWordAddress(baseAddress, b);
			m_AddressSpace.writeWord(dataLocation, c);
			break;
		}

		case VAR_Opcodes::OP226: // storew array byteindex value
		{
			Address baseAddress = a;
			auto dataLocation = baseAddress + b;
			m_AddressSpace.writeByte(dataLocation, static_cast<Byte>(c));
			break;
		}

		case VAR_Opcodes::OP227: // put_prop object property value
		{
			auto object = getObject(a);
			auto block = object.getPropertyBlock(b);

			if(block.getSize() == 1)
			{
				m_AddressSpace.writeByte(block.getAddress(), static_cast<Byte>(c & 0xff));
			}
			else if(block.getSize() == 2)
			{
				m_AddressSpace.writeWord(block.getAddress(), c);
			}
			else
			{
				panic("cannot write value > 2 bytes long to a property block");
			}
			break;

		}

		case VAR_Opcodes::OP228:
		{
			executeVAR_OP228(a, b);			
			break;
		}

		case VAR_Opcodes::OP229: // print_char zascii
		{
			std::string text(1,static_cast<char>(a));
			m_Console->print(text);
			break;
		}

		case VAR_Opcodes::OP230: // print_num value
		{
			m_Console->print(a);
			break;
		}

		case VAR_Opcodes::OP231: // random
		{
			auto variableID = readVariableID();

			auto range = AsSignedWord(a);
			if(range < 0)
			{
				// We need to seed
				std::srand(a);
			}
			else
			{
				// We need a number between 1 and range
				auto randomNumber = static_cast<Word>((std::rand() % range) + 1);
				storeVariable(variableID, randomNumber);
			}

			break;
		}

		case VAR_Opcodes::OP232: // push value
		{
			m_StackSpace.push(a);
			break;
		}

		case VAR_Opcodes::OP233: // pull (variable)
		{
			Byte variableID = static_cast<Byte>(a);
			auto value = m_StackSpace.pop();
			storeVariableInPlace(variableID, value);

			break;
		}

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}

void Story::executeVAR_OP228(Address textAddress, Address parseAddress)
{
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

} // end of namespace