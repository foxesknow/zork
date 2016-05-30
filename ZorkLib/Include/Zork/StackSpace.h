#pragma once

#include <array>

#include <Zork\CoreTypes.h>

namespace zork
{

class StackFrame
{
private:
	unsigned int m_Base;
	Address m_ReturnAddress;
	Word m_ResultVariable;
	size_t m_NumberOfParameters;
	unsigned int m_NumberOfLocals;

public:
	StackFrame(unsigned int base, Address returnAddress, Word resultVariable, size_t numberOfParameters, unsigned int numberOfLocals) 
		: m_Base(base), m_ReturnAddress(returnAddress), m_ResultVariable(resultVariable), m_NumberOfParameters(numberOfParameters), m_NumberOfLocals(numberOfLocals)
	{
		if(m_Base == 2)
		{
			doNothing();
		}
	}

	unsigned int getBase()const
	{
		return m_Base;
	}

	Address getReturnAddress() const
	{
		return m_ReturnAddress;
	}

	Word getResultVariable() const
	{
		return m_ResultVariable;
	}

	size_t getNumberOfParameters() const
	{
		return m_NumberOfParameters;
	}

	unsigned int getNumberOfLocals() const
	{
		return m_NumberOfLocals;
	}

	unsigned int getPushBase() const
	{
		return m_Base + m_NumberOfLocals;
	}
};

class StackSpace
{
private:
	std::array<Word,1024> m_Stack;

	// NOTE: SP Always points to the next free location on the stack
	unsigned int m_SP = 0;

public:

	void push(Word word)
	{
		m_Stack[m_SP++]=word;
	}

	Word pop()
	{
		if(m_SP == 0) panic("the stack is empty!");

		auto value = m_Stack[--m_SP];
		m_Stack[m_SP] = 0xcccc;
		return value;
	}

	Word peek() const
	{
		if(m_SP == 0) panic("the stack is empty!");
		return m_Stack[m_SP-1];
	}

	StackFrame allocateNewFrame(Address returnAddress, size_t numberOfParameters, unsigned int numberOfLocals, Word resultVariable)
	{
		auto sp=m_SP;

		// Reserve space for local variables
		m_SP += numberOfLocals;

		return StackFrame(sp, returnAddress, resultVariable, numberOfParameters, numberOfLocals);
	}

	void revertToFrame(const StackFrame &frameInfo)
	{
		if(frameInfo.getBase() > m_SP)
		{
			throw Exception("cannot revert to a stack address greater than the current stack pointer");
		}

		m_SP=frameInfo.getBase();
	}

	Word getLocal(const StackFrame &frameInfo, unsigned int localIndex)const
	{
		if(localIndex == 0 || localIndex > frameInfo.getNumberOfLocals())
		{
			panic("invalid local index");
		}

		auto zeroBasedIndex = localIndex - 1;
		auto location = frameInfo.getBase() + zeroBasedIndex;
		return m_Stack[location];
	}

	void setLocal(const StackFrame &frameInfo, unsigned int localIndex, Word value)
	{
		if(localIndex == 0 || localIndex > frameInfo.getNumberOfLocals())
		{
			panic("invalid local index");
		}
		
		auto zeroBasedIndex = localIndex - 1;
		auto location = frameInfo.getBase() + zeroBasedIndex;
		m_Stack[location] = value;
	}

	unsigned int getSP() const
	{
		return m_SP;
	}
};

} // end of namespace
