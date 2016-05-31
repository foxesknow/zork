#include <Zork\Story.h>
#include <vector>

namespace zork
{

void Story::executeOP1(const OpcodeDetails &opcodeDetails, OperandType type1)
{
	const auto opcode = static_cast<OP1_Opcodes>(opcodeDetails.getDecodedOpcode());

	switch(opcode)
	{
		case OP1_Opcodes::OP128: 
			handle_jz(type1);
			break;

		case OP1_Opcodes::OP129:
			handle_get_sibling(type1);
			break;

		case OP1_Opcodes::OP130:
			handle_get_child(type1);
			break;

		case OP1_Opcodes::OP131:
			handle_get_parent(type1);
			break;

		case OP1_Opcodes::OP132:
			handle_get_prop_len(type1);
			break;

		case OP1_Opcodes::OP133:
			handle_inc(type1);
			break;

		case OP1_Opcodes::OP134:
			handle_dec(type1);
			break;

		case OP1_Opcodes::OP135:
			handle_print_addr(type1);
			break;

		case OP1_Opcodes::OP136:
			handle_call_1s(type1);
			break;

		case OP1_Opcodes::OP137:
			handle_remove_obj(type1);
			break;

		case OP1_Opcodes::OP138:
			handle_print_obj(type1);
			break;

		case OP1_Opcodes::OP139:
			handle_ret(type1);
			break;

		case OP1_Opcodes::OP140:
			handle_jump(type1);
			break;

		case OP1_Opcodes::OP141:
			handle_print_paddr(type1);
			break;

		case OP1_Opcodes::OP142:
			handle_load(type1);
			break;

		case OP1_Opcodes::OP143:
			if(m_Version < 5)
			{
				handle_not(type1);
			}
			else
			{
				handle_call_1n(type1);
			}
			break;

		default:
			ThrowNotImplemented(opcodeDetails);
			break;
	}
}


void Story::handle_jz(OperandType type1)
{
	// jz a ?(label)

	const auto a = read(type1);

	// Jump if a is 0
	bool outcome = (a == 0);
	auto branchDetails = readBranchDetails();
	if(branchDetails.shouldBranch(outcome)) applyBranch(branchDetails);
}

void Story::handle_get_sibling(OperandType type1)
{
	// get_sibling object -> (result) ? (label);

	const auto a = read(type1);

	auto variableID = readVariableID();
	auto branchDetails = readBranchDetails();

	auto object = getObject(a);
	auto sibling = object.getSibling();
	storeVariable(variableID, sibling);

	if(branchDetails.shouldBranch(sibling != 0)) applyBranch(branchDetails);
}

void Story::handle_get_child(OperandType type1)
{
	// get_child object -> (result) ?(label)

	const auto a = read(type1);

	auto variableID = readVariableID();
	auto branchDetails = readBranchDetails();

	auto object = getObject(a);
	auto child = object.getChild();
	storeVariable(variableID, child);

	if(branchDetails.shouldBranch(child != 0)) applyBranch(branchDetails);
}
void Story::handle_get_parent(OperandType type1)
{
	// get_parent object -> (result)

	const auto a = read(type1);

	auto variableID = readVariableID();
	auto object = getObject(a);
	auto parent = object.getParent();
	storeVariable(variableID, parent);
}

void Story::handle_get_prop_len(OperandType type1)
{
	auto propertyAddress = read(type1);
	auto variableID = readVariableID();

	Word size = Object::getPropertLengthForAddress(m_AddressSpace, propertyAddress, m_Version);

	storeVariable(variableID, static_cast<Word>(size));
}

void Story::handle_inc(OperandType type1)
{
	// inc (variable)
	const auto a = read(type1);

	// TODO: This needs fixing!
	auto variableID = static_cast<Byte>(a);
	auto value = loadVariableInPlace(variableID);

	// Do a signed increase
	SWord adjustedValue = AsSignedWord(value) + 1;
	storeVariableInPlace(variableID, AsWord(adjustedValue));
}

void Story::handle_dec(OperandType type1)
{
	// dec (variable)
	const auto a = read(type1);

	// TODO: This needs fixing
	auto variableID = static_cast<Byte>(a);
	auto value = loadVariableInPlace(variableID);

	// Do a signed decrease
	SWord adjustedValue = AsSignedWord(value) - 1;
	storeVariableInPlace(variableID, AsWord(adjustedValue));
}

void Story::handle_print_addr(OperandType type1)
{
	// print_addr address

	const auto a = read(type1);

	auto text = readString(a);
	m_Console->print(text);
}

void Story::handle_call_1s(OperandType type1)
{
	// call_1s routine -> (result)

	const auto routineAddress = read(type1);

	auto variableID = readVariableID();
	callRoutine(routineAddress, variableID, createArguments({}));
}

void Story::handle_remove_obj(OperandType type1)
{
	// remove_obj object
	const auto objectID = read(type1);
	unlinkObject(objectID);
}

void Story::handle_print_obj(OperandType type1)
{
	// print_obj object

	const auto a = read(type1);

	auto object = getObject(a);
	auto text = readString(object.getNameAddress());
	m_Console->print(text);
}

void Story::handle_ret(OperandType type1)
{
	// ret value

	const auto a = read(type1);
	returnFromCall(a);
}

void Story::handle_jump(OperandType type1)
{
	// jump ?(label)

	const auto a = read(type1);
	auto offset = AsSignedWord(a);
	applyBranch(offset);
}

void Story::handle_print_paddr(OperandType type1)
{
	// print_paddr address

	const auto a = read(type1);
	auto expandedAddress = expandPackedStringAddress(a);
	auto text = readString(expandedAddress);
	m_Console->print(text);
}

void Story::handle_load(OperandType type1)
{
	// load (variable) -> result

	const auto a = read(type1);

	// TODO: This needs fixing
	auto variableID = static_cast<Byte>(a);
	auto value = loadVariableInPlace(variableID);
	auto target = readVariableID();
	storeVariable(target, value);
}

void Story::handle_not(OperandType type1)
{
	// not value -> (result)
	const auto value = read(type1);
	auto variableID = readVariableID();
	storeVariable(variableID, ~value);
}

void Story::handle_call_1n(OperandType type1)
{
	// call_1n routine

	const auto routineAddress = read(type1);
	callRoutine(routineAddress, DiscardResultsVariable, createArguments({}));
}

} // end of namespace