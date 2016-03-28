#pragma once

#include <vector>

#include <Zork\CoreTypes.h>

namespace zork
{

enum class OpcodeForm : Byte
{
	Long,
	Short,
	Variable,
	Extended
};

inline OpcodeForm ToOpcodeForm(Byte opcode)
{
	// The top 2 bits indicate the form
	const int form=opcode >> 6;

	switch(form)
	{
		case 3:		return OpcodeForm::Variable;
		case 2:		return opcode==190 ? OpcodeForm::Extended : OpcodeForm::Short;
		default:	return OpcodeForm::Long;
	}
}

class OpcodeDetails
{
private:
	Byte m_EncodedOpcode;
	Byte m_DecodedOpcode;

public:
	OpcodeDetails(Byte encodedOpcode, Byte decodedOpcode) : m_EncodedOpcode(encodedOpcode), m_DecodedOpcode(decodedOpcode)
	{
	}

	Byte getEncodedOpcode()const
	{
		return m_EncodedOpcode;
	}

	Byte getDecodedOpcode()const
	{
		return m_DecodedOpcode;
	}
};

enum class OperandCount
{
	OP0,
	OP1,
	OP2,
	Variable
};


enum class OperandType : Byte
{
	Large,		// (0 to 65535) 2 bytes
	Small,		// (0 to 255) 1 byte
	Variable,	// 1 byte
	Omitted		// 0 bytes 
};

constexpr OperandType ToOperandType(Byte value)
{
	return value>=0 && value <=2 ? static_cast<OperandType>(value) : OperandType::Omitted;
}

/*
 * NOTE: The opcode notation uses -> to indicate a store variable.
 * It uses ? to indicate a branch. If the ? is omitted then the branch is a short form
 */

enum class OP0_Opcodes
{
	OP176,	// rtrue
	OP177,	// rflase
	OP178,	// print
	OP179,	// print_ret
	OP180,	// nop
	OP181,	// (v1) save, (v4) save, (v5) illegal
	OP182,	// (v1) restore, (v4) restore, (v5) illegal
	OP183,	// restart
	OP184,	// ret_popped
	OP185,	// (v1) pop, (v5-v6) catch
	OP186,	// quit
	OP187,	// new_line
	OP188,	// (v3) show_status, (v4) illegal
	OP189,	// (v3) verify
	OP190,	// (v5) first byte of extended opcode
	OP191,	// (v5-) piracy
};

enum class OP1_Opcodes
{
	OP128,	// jz a
	OP129,	// get_sibling
	OP130,	// get_child
	OP131,	// get_parent
	OP132,	// get_prop_len
	OP133,	// inc
	OP134,	// dec
	OP135,	// print_addr
	OP136,	// call 1s
	OP137,	// remove_obj
	OP138,	// print_obj
	OP139,	// ret
	OP140,	// jump
	OP141,	// print_paddr
	OP142,	// load
	OP143,	// (v1-v4) not, (v5) call_1n
};

} // end of namespace
