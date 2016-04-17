#include <Zork\Story.h>
#include <vector>

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

		case OP1_Opcodes::OP133: // inc (variable)
		{
			auto variableID = static_cast<Byte>(a);
			auto value = loadVariable(variableID);

			// Do a signed increase
			SWord adjustedValue = AsSignedWord(value) + 1;
			storeVariable(variableID, AsWord(adjustedValue));

			break;
		}

		case OP1_Opcodes::OP134: // dec (variable)
		{
			auto variableID = static_cast<Byte>(a);
			auto value = loadVariable(variableID);

			// Do a signed decrease
			SWord adjustedValue = AsSignedWord(value) - 1;
			storeVariable(variableID, AsWord(adjustedValue));
			
			break;
		}

		case OP1_Opcodes::OP135: // print_addr address
		{
			auto text = readString(a);
			m_Console->print(text);
			break;
		}

		case OP1_Opcodes::OP136: // call_1s routine -> (result)
		{
			auto variableID = readVariableID();
			callRoutine(a, variableID, createArguments({}));

			break;
		}

		case OP1_Opcodes::OP137: // remove_obj object
		{
			ThrowNotImplemented();
			break;
		}

		case OP1_Opcodes::OP138: // print_obj object
		{
			auto object = getObject(a);
			auto text = readString(object.getNameAddress());
			m_Console->print(text);
			break;
		}

		case OP1_Opcodes::OP139: // ret value
		{
			returnFromCall(a);
			break;
		}

		case OP1_Opcodes::OP140: // jump ?(label)
		{
			auto offset = AsSignedWord(a);
			applyBranch(offset);
			break;
		}

		case OP1_Opcodes::OP141: // print_paddr address
		{
			auto expandedAddress = expandPackedStringAddress(a);
			auto text = readString(expandedAddress);
			m_Console->print(text);
			break;
		}

		case OP1_Opcodes::OP142: // load (variable) -> result
		{
			auto variableID = static_cast<Byte>(a);
			auto value = loadVariable(variableID);
			auto target = readVariableID();
			storeVariable(target, value);

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