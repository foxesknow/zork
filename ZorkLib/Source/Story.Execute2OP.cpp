#include <algorithm>
#include <Zork\Story.h>

namespace zork
{

void Story::executeOP2(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2)
{
	const auto opcode = static_cast<OP2_Opcodes>(opcodeDetails.getDecodedOpcode());
	
	// NOTE: Special case
	if(opcode == OP2_Opcodes::OP0_UNUSED) return;

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

		case OP2_Opcodes::OP6: // jin obj1 obj2 ?(label)
		{
			// jump is obj1 is a direct child of obj2, i.e., if parent of obj1 is obj2
			auto obj1 = getObject(a);

			bool outcome = (obj1.getParent() == b);
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);

			break;
		}

		case OP2_Opcodes::OP7: // test bitmap flags ?(label)
		{
			// NOTE: All the flags of b must be set
			bool outcome = ((a & b) == a);
			auto branchDetails = readBranchDetails();
			if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);

			break;
		}

		case OP2_Opcodes::OP8: // or a b -> (result)
		{
			Word result = a | b;
			auto variableID = readVariableID();
			storeVariable(variableID, result);
			break;
		}

		case OP2_Opcodes::OP9: // and a b -> (result)
		{
			Word result = a & b;
			auto variableID = readVariableID();
			storeVariable(variableID, result);
			
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

		case OP2_Opcodes::OP13: // store (variable) value
		{
			auto variableID = static_cast<Byte>(a);
			storeVariable(variableID, b);
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

			auto variableID = readVariableID();
			storeVariable(variableID, value);
			
			break;
		};

		case OP2_Opcodes::OP16: // loadb array word-index -> (result)
		{
			Address baseAddress = a;
			Address dataAddress = baseAddress +  b;
			auto value = m_AddressSpace.readByte(dataAddress);

			auto variableID = readVariableID();
			storeVariable(variableID, value);

			break;
		};

		case OP2_Opcodes::OP17: // get_prop object property -> (result)
		{
			auto variableID = readVariableID();
			executeOP2_OP17(a, b, variableID);
			
			break;
		}

		case OP2_Opcodes::OP18: // get_prop_addr object property -> (result);
		{
			auto variableID = readVariableID();
			executeOP2_OP18(a, b, variableID);

			break;
		}

		case OP2_Opcodes::OP19: // get_next_prop object property -> (result);
		{
			auto variableID = readVariableID();
			executeOP2_OP19(a, b, variableID);

			break;
		}

		case OP2_Opcodes::OP20: // add a b -> (result)
		{
			SWord result = AsSignedWord(a) + AsSignedWord(b);
			auto variableID = readVariableID();
			storeVariable(variableID, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP21: // sub a b -> (result)
		{
			SWord result = AsSignedWord(a) - AsSignedWord(b);
			auto variableID = readVariableID();
			storeVariable(variableID, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP22: // mul a b -> (result)
		{
			SWord result = AsSignedWord(a) * AsSignedWord(b);
			auto variableID = readVariableID();
			storeVariable(variableID, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP23: // div a b -> (result)
		{
			if(b == 0) throw Exception("attempt to divide by 0");

			SWord result = AsSignedWord(a) / AsSignedWord(b);
			auto variableID = readVariableID();
			storeVariable(variableID, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP24: // mod a b -> (result)
		{
			if(b == 0) throw Exception("attempt to mod by 0");

			const SWord result = AsSignedWord(a) % AsSignedWord(b);
			auto variableID = readVariableID();
			storeVariable(variableID, AsWord(result));
			
			break;
		}

		case OP2_Opcodes::OP25: // call_2s routine arg1 -> (result);
		{
			if(m_Version >= 4)
			{
				auto variableID = readVariableID();
				auto arguments = createArguments({b});
				callRoutine(a, variableID, arguments);
			}
			else
			{
				panic("call_2s requires v4 or above");
			}

			break;
		}

		case OP2_Opcodes::OP26: // call_2n routine arg1
		{
			if(m_Version >= 5)
			{
				auto arguments = createArguments({b});
				callRoutine(a, DiscardResultsVariable, arguments);
			}
			else
			{
				panic("call_2n requires v5 or above");
			}

			break;
		}

		case OP2_Opcodes::OP27: // set_color
		{
			panic("op2_op27 not implemented");
			break;
		}

		case OP2_Opcodes::OP28: // throw value stack-frame
		{
			unwindToFrame(b);
			returnFromCall(a);
			break;
		}

		default:
			ThrowNotImplemented();
			break;
	}
}

void Story::executeOP2_OP17(Word objectID, Word propertyID, Byte variableID)
{
	Word result = 0;
	auto object = getObject(objectID);
	auto allProperties = object.getAllPropertyBlocks();

	auto it = std::find_if
	(
		allProperties.begin(),
		allProperties.end(),
		[propertyID](const PropertyBlock &block) {return block.getID() == propertyID;}
	);

	if(it != allProperties.end())
	{
		const auto &block = (*it);
		if(block.getSize() == 1)
		{
			result = m_AddressSpace.readByte(block.getAddress());
		}
		else if(block.getSize() == 2)
		{
			result = m_AddressSpace.readWord(block.getAddress());
		}
		else
		{
			// It's unspecified what to do, so fail
			panic("cant read a property greater than 2 bytes");
		}
	}
	else
	{
		// The object doesn't have this property, so get it from the defaults
		if(propertyID < getNumberOfDefaultProperties())
		{
			result = getDefaultProperty(propertyID);
		}
		else
		{
			panic("invalid default property id");
		}
	}

	storeVariable(variableID, result);
}

void Story::executeOP2_OP18(Word objectID, Word propertyID, Byte variableID)
{
	auto object = getObject(objectID);
	auto allProperties = object.getAllPropertyBlocks();

	auto it = std::find_if
	(
		allProperties.begin(),
		allProperties.end(),
		[propertyID](const PropertyBlock &block) {return block.getID() == propertyID;}
	);

	if(it != allProperties.end())
	{
		storeVariable(variableID, static_cast<Word>((*it).getAddress()));
	}
	else
	{
		// If the property doesn't exist then we store 0
		storeVariable(variableID, 0);
	}
}

void Story::executeOP2_OP19(Word objectID, Word propertyID, Byte variableID)
{
	Word result = 0;
	auto object = getObject(objectID);
	auto allProperties = object.getAllPropertyBlocks();

	if(propertyID == 0)
	{
		// This means get the first property number
		if(allProperties.size() == 0) panic("op2_op19: no properties");

		result = allProperties[0].getID();
	}
	else
	{
		auto it = std::find_if
		(
			allProperties.begin(),
			allProperties.end(),
			[propertyID](const PropertyBlock &block) {return block.getID() == propertyID; }
		);

		// It's an error to request a property that doesn't exist
		if(it == allProperties.end()) panic("op2_op19: property does not exist");

		// We want the property after the one that matches the supplied id
		++it;
		if(it != allProperties.end())
		{
			result = (*it).getID();
		}
		else
		{
			// There aren't any more left, so the ID is 0
			result = 0;
		}
	}

	storeVariable(variableID, result);
}

} // end of namespace