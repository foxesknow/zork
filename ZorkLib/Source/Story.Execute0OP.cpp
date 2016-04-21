#include <Zork\Story.h>

namespace zork
{

void Story::executeOP0(OpcodeDetails opcodeDetails)
{
	const auto opcode = static_cast<OP0_Opcodes>(opcodeDetails.getDecodedOpcode());

	switch(opcode)
	{
		case OP0_Opcodes::OP176: // rtrue
		{
			returnFromCall(1);
			break;
		}

		case OP0_Opcodes::OP177: // rfalse
		{
			returnFromCall(0);
			break;
		}

		case OP0_Opcodes::OP178: // print
		case OP0_Opcodes::OP179: // print_ret
		{
			ZsciiReader reader(m_AddressSpace, m_PC);
			auto string = readString(reader);
			m_Console->print(string);

			if(opcode == OP0_Opcodes::OP178)
			{
				m_PC = increaseWordAddress(reader.getAddress(), 1); // +1 as the address is the end of the string
			}
			else
			{
				returnFromCall(1);
			}

			break;
		}

		case OP0_Opcodes::OP180: // nop
		{
			// Do nothing!
			break;
		}

		case OP0_Opcodes::OP181:
		{
			ThrowNotImplemented();
			break;
		}

		case OP0_Opcodes::OP182:
		{
			ThrowNotImplemented();
			break;
		}

		case OP0_Opcodes::OP183:
		{
			ThrowNotImplemented();
			break;
		}

		case OP0_Opcodes::OP184: // ret_popped
		{
			// Return the item on the top of the stack
			auto returnValue = loadVariable(0);
			returnFromCall(returnValue);
			break;
		}

		case OP0_Opcodes::OP185: // (v1) pop, (v5-v6) catch
		{
			executeOP0_OP185();
			break;
		}

		case OP0_Opcodes::OP186:
		{
			ThrowNotImplemented();
			break;
		}

		case OP0_Opcodes::OP187: // new_line
		{
			m_Console->newline();
			break;
		}

		case OP0_Opcodes::OP188:
		{
			ThrowNotImplemented();
			break;
		}

		case OP0_Opcodes::OP189: // verify ?(label)
		{
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

			break;
		}

		case OP0_Opcodes::OP190:
		{
			// We shouldn't get here as executeNextInstruction should have handled this
			panic("op0_op190 should have been handled by executeNextInstruction");
			break;
		}

		case OP0_Opcodes::OP191: // piracy ?(label)
		{
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

			break;
		}

		default:
		{
			ThrowNotImplemented();
			break;
		}
	}
}

void Story::executeOP0_OP185()
{
	if(m_Version < 4) // pop
	{
		// It's a pop. Just throw away the value
		m_StackSpace.pop();
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

} // end of namespace