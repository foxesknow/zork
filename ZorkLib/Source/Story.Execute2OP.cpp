#include <Zork\Story.h>

namespace zork
{

void Story::executeOP2(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2)
{
	const auto opcode = static_cast<OP2_Opcodes>(opcodeDetails.getDecodedOpcode());

	const auto a = read(type1);
	const auto b = read(type2);

	switch(opcode)
	{
		case OP2_Opcodes::OP1:	// je a b ?(label)
		{
			bool outcome = (AsSignedWord(a) == AsSignedWord(b));
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
			break;
		}

		case OP2_Opcodes::OP2:	// jl a b ?(label)
		{
			bool outcome = (AsSignedWord(a) < AsSignedWord(b));
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
			break;
		}

		case OP2_Opcodes::OP3:	// jg a b ?(label)
		{
			bool outcome = (AsSignedWord(a) > AsSignedWord(b));
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
			break;
		}

		case OP2_Opcodes::OP4: // dec_chk (variable) value ?(label)
		{
			auto variableID = static_cast<Byte>(a);
			auto value = AsSignedWord(loadVariable(variableID));
			value--;
			storeVariable(variableID, AsWord(value));

			bool outcome = (value < AsSignedWord(b));
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
			break;
		}

		case OP2_Opcodes::OP5: // inc_chk (variable) value ?(label)
		{
			auto variableID = static_cast<Byte>(a);
			auto value = AsSignedWord(loadVariable(variableID));
			value++;
			storeVariable(variableID, AsWord(value));

			bool outcome = (value > AsSignedWord(b));
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
			
			break;
		}

		case OP2_Opcodes::OP6: // jin
		{
			// TODO
			throw Exception("not implemented");
			break;
		}

		case OP2_Opcodes::OP7: // test bitmap flags ?(label)
		{
			bool outcome = ((a & b) == a);
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);

			break;
		}

		case OP2_Opcodes::OP8: // or a b -> (result)
		{
			Word result = a | b;
			auto variable = readVariableID();
			storeVariable(variable, result);
			break;
		}

		case OP2_Opcodes::OP9: // and a b -> (result)
		{
			Word result = a & b;
			auto variable = readVariableID();
			storeVariable(variable, result);
			
			break;
		}

		case OP2_Opcodes::OP10: // test_attr object attribute ?(label)
		{
			auto object = getObject(a);
			bool outcome = object.getFlag(b);

			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);

			break;
		}

		case OP2_Opcodes::OP11: // set_attr object attribute
		{
			auto object = getObject(a);
			object.setFlag(b, true);

			break;
		}

		case OP2_Opcodes::OP12: // clear_attr object attribute
		{
			auto object = getObject(a);
			object.setFlag(b, false);

			break;
		}

		case OP2_Opcodes::OP13: // store
		{
			auto variable = static_cast<Byte>(a);
			storeVariable(variable, b);
			break;
		}

		case OP2_Opcodes::OP14: // insert_obj object destination
		{
			auto child = getObject(a);
			auto parent = getObject(b);
			parent.insertObject(child);

			break;
		}

		case OP2_Opcodes::OP15: // loadw array word-index -> (result)
		{
			Address baseAddress = a;
			auto dataAddress = increaseWordAddress(baseAddress, b);
			auto value = m_AddressSpace.readWord(dataAddress);

			auto variable = readVariableID();
			storeVariable(variable, value);
			
			break;
		};

		case OP2_Opcodes::OP16: // loadb array word-index -> (result)
		{
			Address baseAddress = a;
			Address dataAddress = baseAddress +  b;
			auto value = m_AddressSpace.readByte(dataAddress);

			auto variable = readVariableID();
			storeVariable(variable, value);

			break;
		};

		case OP2_Opcodes::OP17: // get_prop object property -> (result)
		{
			// TODO
			ThrowNotImplemented();
			break;
		}

		case OP2_Opcodes::OP20: // add a b -> (result)
		{
			SWord result = AsSignedWord(a) + AsSignedWord(b);
			auto variable = readVariableID();
			storeVariable(variable, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP21: // sub a b -> (result)
		{
			SWord result = AsSignedWord(a) - AsSignedWord(b);
			auto variable = readVariableID();
			storeVariable(variable, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP22: // mul a b -> (result)
		{
			SWord result = AsSignedWord(a) * AsSignedWord(b);
			auto variable = readVariableID();
			storeVariable(variable, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP23: // div a b -> (result)
		{
			if(b == 0) throw Exception("attempt to divide by 0");

			SWord result = AsSignedWord(a) / AsSignedWord(b);
			auto variable = readVariableID();
			storeVariable(variable, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP24: // mod a b -> (result)
		{
			if(b == 0) throw Exception("attempt to mod by 0");

			const SWord result = AsSignedWord(a) % AsSignedWord(b);
			auto variable = readVariableID();
			storeVariable(variable, AsWord(result));
			
			break;
		}
	}

}

} // end of namespace