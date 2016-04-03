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
	Address m_BaseAddress;
	Byte m_EncodedOpcode;
	Byte m_DecodedOpcode;

public:
	OpcodeDetails(Address baseAddress, Byte encodedOpcode, Byte decodedOpcode) : m_BaseAddress(baseAddress), m_EncodedOpcode(encodedOpcode), m_DecodedOpcode(decodedOpcode)
	{
	}

	/** The PC address where the opcode was read from */
	Address getBaseAddress()const
	{
		return m_BaseAddress;
	}

	/** The opcode as it was fully encoded */
	Byte getEncodedOpcode()const
	{
		return m_EncodedOpcode;
	}

	/** The opcode after any type information has been stripped away */
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
 * For Long 2OP opcodes bit 6 is the type of the first operand, bit 5 the type of the second.
 * If the bit is 0 it means a small constant, 1 means a variable
 */
constexpr Byte MakeLong_2OP_SC_SC(Byte opcode)
{
	return opcode & 31;	// The bottom 5 bits is the opcode
}

constexpr Byte MakeLong_2OP_SC_VAR(Byte opcode)
{
	return (opcode & 31) | (1<<5);	
}

constexpr Byte MakeLong_2OP_VAR_SC(Byte opcode)
{
	return (opcode & 31) | (1<<6);
}

constexpr Byte MakeLong_2OP_VAR_VAR(Byte opcode)
{
	return (opcode & 31) | (1<<5) | (1<<6);
}

/*
 * For short 1OP opcodes bit 4 and 5 give the opcode type
 */
constexpr Byte MakeShort_1OP_LC(Byte opcode)
{
	return (2 << 6) | (opcode & 15) | static_cast<Byte>(OperandType::Large)<<4;
}

constexpr Byte MakeShort_1OP_SC(Byte opcode)
{
	return (2 << 6) | (opcode & 15) | static_cast<Byte>(OperandType::Small)<<4;
}

constexpr Byte MakeShort_1OP_VAR(Byte opcode)
{
	return (2 << 6) | (opcode & 15) | static_cast<Byte>(OperandType::Variable)<<4;
}

constexpr Byte MakeShort_0OP(Byte opcode)
{
	return (2 << 6) | (opcode & 15) | static_cast<Byte>(OperandType::Omitted)<<4;
}

/*
 * For variable opcodes the 5 bit is 1 vor variable, and 0 for 2 operands
 */
constexpr Byte MakeVariable_2OP(Byte opcode)
{
	return (3 << 6) | (opcode & 31);
}

constexpr Byte MakeVariable_VAR(Byte opcode)
{
	return (3 << 6) | (opcode & 31) | (1 << 5);
}

/*******************************************************************************
$00 -- $1f long 2OP small constant, small constant
$20 -- $3f long 2OP small constant, variable
$40 -- $5f long 2OP variable, small constant
$60 -- $7f long 2OP variable, variable
$80 -- $8f short 1OP large constant
$90 -- $9f short 1OP small constant
$a0 -- $af short 1OP variable
$b0 -- $bf short 0OP
except $be extended opcode given in next byte
$c0 -- $df variable 2OP (operand types in next byte)
$e0 -- $ff variable VAR (operand types in next byte(s))
*******************************************************************************/


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

enum class OP2_Opcodes
{
	OP0_UNUSED,
	OP1,	// je
	OP2,	// jl
	OP3,	// jg
	OP4,	// dec_chk
	OP5,	// inc_chk
	OP6,	// jin
	OP7,	// test
	OP8,	// or
	OP9,	// and
	OP10,	// test_attr
	OP11,	// set_attr
	OP12,	// clear_Attr
	OP13,	// store
	OP14,	// insert_obj
	OP15,	// loadw
	OP16,	// loadb
	OP17,	// get_prop
	OP18,	// get_prop_addr
	OP19,	// get_next_prop
	OP20,	// add
	OP21,	// sub
	OP22,	// mul
	OP23,	// div
	OP24,	// mod
	OP25,	// (v4) call_2s
	OP26,	// (v5) call_2n
	OP27,	// (v5) set_color, (v6) set_color
	OP28,	// (v5/6) throw
};


class BranchDetails
{
private:
	SWord m_Offset;
	bool m_ComparisonValue;

public:
	BranchDetails(SWord offset, bool comparisonValue) : m_Offset(offset), m_ComparisonValue(comparisonValue)
	{
	}

	SWord getOffset()const
	{
		return m_Offset;
	}

	bool getComparisonValue()const
	{
		return m_ComparisonValue;
	}

	bool shouldBranch(bool comparisonValue)const
	{
		return m_ComparisonValue == comparisonValue;
	}
};

} // end of namespace
