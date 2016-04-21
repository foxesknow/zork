#pragma once

#include <map>
#include <set>
#include <vector>
#include <stack>
#include <memory>
#include <sstream>

#include <Zork\AddressSpace.h>
#include <Zork\StackSpace.h>
#include <Zork\ZsciiReader.h>
#include <Zork\Object.h>
#include <Zork\Instructions.h>
#include <Zork\Console.h>

namespace zork
{

class Story
{
private:
	mutable AddressSpace m_AddressSpace;
	std::shared_ptr<Console> m_Console;

	Byte m_Version;
	
	StackSpace m_StackSpace;
	std::stack<StackFrame> m_Frames;
	
	Address m_StaticBase;
	Address m_GlobalVariablesTable;
	Address m_PC;

	std::map<int,std::string> m_Abbreviations;
	std::set<std::string> m_Dictionary;

private:
	void ThrowNotImplemented() const
	{
		std::stringstream stream;
		stream << "not implemented - PC = " << std::hex << m_PC;
		throw Exception(stream.str());
	}

	int getFileSize() const;

	void buildAbbreviationCache();
	const std::string &getAbbreviation(int id)const;
	std::string readAbbreviation(int addreviationNumber)const;

	void buildDictionary();

	Address expandPackedRoutineAddress(Address address)const;
	Address expandPackedStringAddress(Address address)const;


	void resolveCharacter(std::string &text, const char *&alphabet, ZsciiReader &reader)const;

	Address getObjectTreeBaseAddress()const;

	Word getDefaultProperty(Word propertyID)const;
	Word getNumberOfDefaultProperties()const;

	void executeOP0(OpcodeDetails opcodeDetails);
	void executeOP1(OpcodeDetails opcodeDetails, OperandType type1);
	void executeOP2(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2);
	void executeVAR(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4);
	void executeEXT(OpcodeDetails opcodeDetails, OperandType type1, OperandType type2, OperandType type3, OperandType type4);

	void executeOP0_OP185();

	void executeOP2_OP17(Word objectID, Word propertyID, Byte variableID);
	void executeOP2_OP18(Word objectID, Word propertyID, Byte variableID);
	void executeOP2_OP19(Word objectID, Word propertyID, Byte variableID);

	void storeVariable(Byte variableID, Word value);
	Word loadVariable(Byte variableID);

	Byte readNextByte()
	{
		return m_AddressSpace.readByte(m_PC++);
	}

	Word readNextWord()
	{
		Word value=m_AddressSpace.readWord(m_PC);
		m_PC=increaseWordAddress(m_PC, 1);
		return value;
	}

	Byte readVariableID()
	{
		return readNextByte();
	}

	StackFrame allocateNewFrame(Address returnAddress, unsigned int numberOfLocals, Word returnVariable)
	{
		auto stackFrame = m_StackSpace.allocateNewFrame(returnAddress, numberOfLocals, returnVariable);
		m_Frames.push(stackFrame);

		return stackFrame;
	}

	Word read(OperandType operandType);

	BranchDetails readBranchDetails();
	
	void applyBranch(const BranchDetails &branchDetails);
	void applyBranch(SWord offset);

	void callRoutine(Address routineAddress, Word returnVariable, const std::vector<Word> &arguments);
	void returnFromCall(Word result);
	void unwindToFrame(Word frameID);

	std::vector<Word> createArguments(std::initializer_list<Word> arguments)
	{
		return std::vector<Word>(arguments);
	}

	Story(const Story &) = delete;
	Story(Story &&) = delete;
	Story &operator=(const Story &) = delete;
	Story &operator=(Story &&) = delete;

public:
	Story(AddressSpace &&addressSpace, std::shared_ptr<Console> console);
	

	// For now...
	std::string readString(Address address)const;
	std::string readString(ZsciiReader &reader)const;

	Object getObject(Word objectID)const;

	Word getNumberOfObjects()const;

	void run();
	void executeNextInstruction();
	OpcodeDetails decode(OpcodeForm &opcodeForm, OperandCount &operandCount);
	
};


} // end of namespace