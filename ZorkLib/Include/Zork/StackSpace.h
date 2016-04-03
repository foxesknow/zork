#pragma once

#include <array>

#include <Zork\CoreTypes.h>

namespace zork
{

class FrameInfo
{
private:
	unsigned int m_Base;
	Address m_ReturnAddress;
	Byte m_ResultVariable;

public:
	FrameInfo(unsigned int base, Address returnAddress, Byte resultVariable) : m_Base(base), m_ReturnAddress(returnAddress), m_ResultVariable(resultVariable)
	{
	}

	unsigned int getBase()const
	{
		return m_Base;
	}

	Address getReturnAddress()const
	{
		return m_ReturnAddress;
	}

	Byte getResultVariable()const
	{
		return m_ResultVariable;
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
		return m_Stack[--m_SP];
	}

	FrameInfo allocateNewFrame(Address returnAddress, unsigned int numberOfLocals, Byte resultVariable)
	{
		auto sp=m_SP;

		// Reserve space for local variables
		m_SP += numberOfLocals;

		return FrameInfo(sp, returnAddress, resultVariable);
	}

	void revertToFrame(const FrameInfo &frameInfo)
	{
		if(frameInfo.getBase() > m_SP)
		{
			throw Exception("cannot revert to a stack address greater than the current stack pointer");
		}

		m_SP=frameInfo.getBase();
	}

	Word getLocal(const FrameInfo &frameInfo, unsigned int localIndex)const
	{
		auto zeroBasedIndex = localIndex - 1;
		auto location = frameInfo.getBase() + zeroBasedIndex;
		return m_Stack[location];
	}

	void setLocal(const FrameInfo &frameInfo, unsigned int localIndex, Word value)
	{
		auto zeroBasedIndex = localIndex - 1;
		auto location = frameInfo.getBase() + zeroBasedIndex;
		m_Stack[location] = value;
	}
};

} // end of namespace
