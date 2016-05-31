#include <algorithm>
#include <Zork\Story.h>

namespace zork
{

void Story::executeOP2(const OpcodeDetails &opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
	const auto opcode = static_cast<OP2_Opcodes>(opcodeDetails.getDecodedOpcode());
	
	// NOTE: Special case
	if(opcode == OP2_Opcodes::OP0_UNUSED) return;

	switch(opcode)
	{
		case OP2_Opcodes::OP1:
			handle_je(type1, type2, type3, type4);
			break;

		case OP2_Opcodes::OP2:
			handle_jl(type1,type2);
			break;

		case OP2_Opcodes::OP3:
			handle_jg(type1, type2);
			break;

		case OP2_Opcodes::OP4:
			handle_dec_chk(type1, type2);
			break;

		case OP2_Opcodes::OP5:
			handle_inc_chk(type1, type2);
			break;

		case OP2_Opcodes::OP6:
			handle_jin(type1, type2);
			break;

		case OP2_Opcodes::OP7:
			handle_test(type1, type2);
			break;

		case OP2_Opcodes::OP8:
			handle_or(type1, type2);
			break;

		case OP2_Opcodes::OP9:
			handle_and(type1, type2);
			break;

		case OP2_Opcodes::OP10:
			handle_test_attr(type1, type2);
			break;

		case OP2_Opcodes::OP11:
			handle_set_attr(type1, type2);
			break;

		case OP2_Opcodes::OP12:
			handle_clear_attr(type1, type2);
			break;

		case OP2_Opcodes::OP13:
			handle_store(type1, type2);
			break;

		case OP2_Opcodes::OP14:
			handle_insert_obj(type1, type2);
			break;

		case OP2_Opcodes::OP15:
			handle_loadw(type1, type2);
			break;

		case OP2_Opcodes::OP16:
			handle_loadb(type1, type2);
			break;

		case OP2_Opcodes::OP17:
			handle_get_prop(type1, type2);
			break;

		case OP2_Opcodes::OP18:
			handle_get_prop_addr(type1, type2);
			break;

		case OP2_Opcodes::OP19:
			handle_get_next_prop(type1, type2);
			break;

		case OP2_Opcodes::OP20:
			handle_add(type1, type2);
			break;

		case OP2_Opcodes::OP21:
			handle_sub(type1, type2);
			break;

		case OP2_Opcodes::OP22:
			handle_mul(type1, type2);
			break;

		case OP2_Opcodes::OP23:
			handle_div(type1, type2);
			break;

		case OP2_Opcodes::OP24:
			handle_mod(type1, type2);
			break;

		case OP2_Opcodes::OP25:
			handle_call_2s(type1, type2);
			break;

		case OP2_Opcodes::OP26:
			handle_call_2n(type1, type2);
			break;

		case OP2_Opcodes::OP28:
			handle_throw(type1, type2);
			break;

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}

void Story::handle_je(OperandType type1, OperandType type2, OperandType type3, OperandType type4)
{
	// je a b ?(label)
	const Word a = (IsPresent(type1) ? read(type1) : 0);

	auto value = AsSignedWord(a);	
	bool match = false;

	if(IsPresent(type2)) match |= (value == AsSignedWord(read(type2)));
	if(IsPresent(type3)) match |= (value == AsSignedWord(read(type3)));
	if(IsPresent(type4)) match |= (value == AsSignedWord(read(type4)));

	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(match)) applyBranch(branchDetails);
}

void Story::handle_jl(OperandType type1, OperandType type2)
{
	// jl a b ?(label)
	const Word a = read(type1);
	const Word b = read(type2);

	bool outcome = (AsSignedWord(a) < AsSignedWord(b));
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_jg(OperandType type1, OperandType type2)
{
	// jg a b ?(label)
	const Word a = read(type1);
	const Word b = read(type2);

	bool outcome = (AsSignedWord(a) > AsSignedWord(b));
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_dec_chk(OperandType type1, OperandType type2)
{
	// dec_chk (variable) value ?(label)
	const Word a = read(type1);
	const Word b = read(type2);

	auto variableID = static_cast<Byte>(a);
	auto value = AsSignedWord(loadVariableInPlace(variableID));
	value--;
	storeVariableInPlace(variableID, AsWord(value));

	bool outcome = (value < AsSignedWord(b));
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_inc_chk(OperandType type1, OperandType type2)
{
	// inc_chk (variable) value ?(label)
	const Word a = read(type1);
	const Word b = read(type2);

	auto variableID = static_cast<Byte>(a);
	auto value = AsSignedWord(loadVariableInPlace(variableID));
	value++;
	storeVariableInPlace(variableID, AsWord(value));

	bool outcome = (value > AsSignedWord(b));
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_jin(OperandType type1, OperandType type2)
{
	// jin obj1 obj2 ?(label)
	const Word a = read(type1);
	const Word b = read(type2);

	// jump if obj1 is a direct child of obj2, i.e., if parent of obj1 is obj2
	auto obj1 = getObject(a);

	bool outcome = (obj1.getParent() == b);
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_test(OperandType type1, OperandType type2)
{
	// test bitmap flags ?(label)
	const Word bitmap = read(type1);
	const Word flags = read(type2);

	// NOTE: All the flags must be set
	bool outcome = ((bitmap & flags) == flags);
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_or(OperandType type1, OperandType type2)
{
	// or a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);

	Word result = a | b;
	auto variableID = readVariableID();
	storeVariable(variableID, result);	
}

void Story::handle_and(OperandType type1, OperandType type2)
{
	// and a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	Word result = a & b;
	auto variableID = readVariableID();
	storeVariable(variableID, result);
}

void Story::handle_test_attr(OperandType type1, OperandType type2)
{
	// test_attr object attribute ?(label)
	const Word a = read(type1);
	const Word b = read(type2);	

	auto object = getObject(a);
	bool outcome = object.getFlag(b);

	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_set_attr(OperandType type1, OperandType type2)
{	
	// set_attr object attribute
	const Word a = read(type1);
	const Word b = read(type2);	

	auto object = getObject(a);
	object.setFlag(b, true);
}

void Story::handle_clear_attr(OperandType type1, OperandType type2)
{
	// clear_attr object attribute
	const Word a = read(type1);
	const Word b = read(type2);	

	auto object = getObject(a);
	object.setFlag(b, false);
}

void Story::handle_store(OperandType type1, OperandType type2)
{
	// store (variable) value
	const Word a = read(type1);
	const Word b = read(type2);	

	// TODO: Fix this

	auto variableID = static_cast<Byte>(a);
	storeVariableInPlace(variableID, b);
}

void Story::handle_insert_obj(OperandType type1, OperandType type2)
{
	// insert_obj object destination
	const Word object = read(type1);
	const Word destination = read(type2);	

	auto newSibling = getObject(destination).getChild();

	unlinkObject(object);
	getObject(destination).setChild(object);
	getObject(object).setParent(destination);
	getObject(object).setSibling(newSibling);

	auto o = getObject(object);
	auto d = getObject(destination);
}

void Story::handle_loadw(OperandType type1, OperandType type2)
{
	// loadw array word-index -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	Address baseAddress = a;
	auto dataAddress = increaseWordAddress(baseAddress, b);
	auto value = m_AddressSpace.readWord(dataAddress);

	auto variableID = readVariableID();
	storeVariable(variableID, value);
}

void Story::handle_loadb(OperandType type1, OperandType type2)
{
	// loadb array word-index -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	Address baseAddress = a;
	Address dataAddress = baseAddress + b;
	auto value = m_AddressSpace.readByte(dataAddress);

	auto variableID = readVariableID();
	storeVariable(variableID, value);	
}

void Story::handle_get_prop(OperandType type1, OperandType type2)
{
	// get_prop object property -> (result)

	const Word objectID = read(type1);
	const Word propertyID = read(type2);	

	auto variableID = readVariableID();
	
	Word result = 0;
	auto object = getObject(objectID);
	auto allProperties = object.getAllPropertyBlocks();

	auto nameAddress = object.getNameAddress();
	auto nameOfObject = readString(nameAddress);

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
		result = getDefaultProperty(propertyID);
	}

	storeVariable(variableID, result);
}

void Story::handle_get_prop_addr(OperandType type1, OperandType type2)
{
	// get_prop_addr object property -> (result);
	const Word objectID = read(type1);
	const Word propertyID = read(type2);	
	
	auto variableID = readVariableID();
	
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

void Story::handle_get_next_prop(OperandType type1, OperandType type2)
{
	// get_next_prop object property -> (result);
	const Word objectID = read(type1);
	const Word propertyID = read(type2);	

	auto variableID = readVariableID();

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

void Story::handle_add(OperandType type1, OperandType type2)
{	
	// add a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	SWord result = AsSignedWord(a) + AsSignedWord(b);
	auto variableID = readVariableID();
	storeVariable(variableID, AsWord(result));
}

void Story::handle_sub(OperandType type1, OperandType type2)
{
	// sub a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	SWord result = AsSignedWord(a) - AsSignedWord(b);
	auto variableID = readVariableID();
	storeVariable(variableID, AsWord(result));
}

void Story::handle_mul(OperandType type1, OperandType type2)
{
	// mul a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	SWord result = AsSignedWord(a) * AsSignedWord(b);
	auto variableID = readVariableID();
	storeVariable(variableID, AsWord(result));
}

void Story::handle_div(OperandType type1, OperandType type2)
{
	// div a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	if(b == 0) throw Exception("attempt to divide by 0");

	SWord result = AsSignedWord(a) / AsSignedWord(b);
	auto variableID = readVariableID();
	storeVariable(variableID, AsWord(result));
}

void Story::handle_mod(OperandType type1, OperandType type2)
{
	// mod a b -> (result)
	const Word a = read(type1);
	const Word b = read(type2);	

	if(b == 0) throw Exception("attempt to mod by 0");

	const SWord result = AsSignedWord(a) % AsSignedWord(b);
	auto variableID = readVariableID();
	storeVariable(variableID, AsWord(result));
}

void Story::handle_call_2s(OperandType type1, OperandType type2)
{
	// call_2s routine arg1 -> (result);
	const Word a = read(type1);
	const Word b = read(type2);	

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
}

void Story::handle_call_2n(OperandType type1, OperandType type2)
{
	// call_2n routine arg1
	const Word a = read(type1);
	const Word b = read(type2);	

	if(m_Version >= 5)
	{
		auto arguments = createArguments({b});
		callRoutine(a, DiscardResultsVariable, arguments);
	}
	else
	{
		panic("call_2n requires v5 or above");
	}
}

void Story::handle_throw(OperandType type1, OperandType type2)
{
	// throw value stack-frame

	const Word a = read(type1);
	const Word b = read(type2);	

	unwindToFrame(b);
	returnFromCall(a);
}




} // end of namespace