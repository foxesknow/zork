#pragma once

#include <vector>
#include <iostream>

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

std::ostream &operator<<(std::ostream &stream, OpcodeForm opcodeForm);

enum class OperandCount
{
	OP0,
	OP1,
	OP2,
	Variable
};

std::ostream &operator<<(std::ostream &stream, OperandCount operandCount);


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

	OpcodeForm m_OpcodeForm;
	OperandCount m_OperandCount;

public:
	OpcodeDetails(Address baseAddress, Byte encodedOpcode, Byte decodedOpcode, OpcodeForm opcodeForm, OperandCount operandCount) 
		: m_BaseAddress(baseAddress), m_EncodedOpcode(encodedOpcode), m_DecodedOpcode(decodedOpcode), m_OpcodeForm(opcodeForm), m_OperandCount(operandCount)
	{
	}

	/** The PC address where the opcode was read from */
	Address getBaseAddress() const
	{
		return m_BaseAddress;
	}

	/** The opcode as it was fully encoded */
	Byte getEncodedOpcode() const
	{
		return m_EncodedOpcode;
	}

	/** The opcode after any type information has been stripped away */
	Byte getDecodedOpcode() const
	{
		return m_DecodedOpcode;
	}

	/** Returns the form of the opcode */
	OpcodeForm getOpcodeForm() const
	{
		return m_OpcodeForm;
	}

	/** Returns the number of operands */
	OperandCount getOperandCount() const
	{
		return m_OperandCount;
	}
};

std::ostream &operator<<(std::ostream &stream, OpcodeDetails opcodeDetails);


enum class OperandType : Byte
{
	Large,		// (0 to 65535) 2 bytes
	Small,		// (0 to 255) 1 byte
	Variable,	// 1 byte
	Omitted		// 0 bytes 
};


std::ostream &operator<<(std::ostream &stream, OperandType operandType);

constexpr OperandType ToOperandType(Byte value)
{
	return value>=0 && value <=2 ? static_cast<OperandType>(value) : OperandType::Omitted;
}

constexpr bool IsPresent(OperandType type)
{
	return type != OperandType::Omitted;
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

enum class VAR_Opcodes
{
	OP224,	// (v1) call, (v4) call_vs
	OP225,	// storew
	OP226,	// storeb
	OP227,	// put_prop
	OP228,	// (v1) sread, (v4) sread, (v5) aread
	OP229,	// print_char
	OP230,	// print_num
	OP231,	// random
	OP232,	// push
	OP233,	// (v1) pull, (v6) pull
	OP234,	// (v3) split_window
	OP235,	// (v3) set_window
	OP236,	// (v4) call_vs2
	OP237,	// (v4) erase_window
	OP238,	// (v4/-) erase_line
	OP239,	// (v4) set_cursor, (v6) set_cursor
	OP240,	// (v4) get_cursor
	OP241,	// (v4) set_text_style
	OP242,	// (v4) buffer_mode
	OP243,	// (v3) output_stream, (v5) output_stream, (v6) output_stream
	OP244,	// (v3) input_stream
	OP245,	// (v5/3) sound_effect
	OP246,	// (v4) read_char
	OP247,	// (v4) scan_table
	OP248,	// (v5/6) not
	OP249,	// (v5) call_vn
	OP250,	// (v5) call_vn2
	OP251,	// (v5) tokenize
	OP252,	// (v5) encode_text
	OP253,	// (v5) copy_table
	OP254,	// (v5) print_table
	OP255,	// (v5) check_arg_count
};

enum class EXT_Opcodes
{
	OP0,	// (v5) save
	OP1,	// (v5) restore
	OP2,	// (v5) log_shift
	OP3,	// (v5) art_shift
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

	/** The offset of the branch */
	SWord getOffset()const
	{
		return m_Offset;
	}

	/** The value the comparison should equate to */
	bool getComparisonValue()const
	{
		return m_ComparisonValue;
	}

	/** Returns true if the branch should be taken, otherwise false */
	bool shouldBranch(bool outcomeOfComparison)const
	{
		return m_ComparisonValue == outcomeOfComparison;
	}
};

} // end of namespace
