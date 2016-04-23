#include <cstdlib>
#include <Zork\Story.h>

namespace zork
{

void Story::executeVAR(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
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

		case VAR_Opcodes::OP226: // storew arrab word-index value
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
			ThrowNotImplemented(opcodeDetails);
			break;
		}

		case VAR_Opcodes::OP229: // print_char zascii
		{
			//ThrowNotImplemented();
			m_Console->print("#");
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
				auto randomNumber = static_cast<Word>((std::rand() / range) + 1);
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
			storeVariable(variableID, value);

			break;
		}

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}

} // end of namespace