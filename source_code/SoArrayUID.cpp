//----------------------------------------------------------------
#include "SoArrayUID.h"
#include "SoMemoryHelp.h"
#include <string> //为了能够调用 malloc free memcpy 等函数
//----------------------------------------------------------------
SoArrayUID::SoArrayUID()
:m_pElementBuff(0)
,m_pStatusBuff(0)
,m_nCapacity(0)
,m_nUsingElementCount(0)
,m_nElementSize(0)
,m_nAddCount(0)
{

}
//----------------------------------------------------------------
SoArrayUID::~SoArrayUID()
{
	if (m_pElementBuff)
	{
		free(m_pElementBuff);
		m_pElementBuff = 0;
		m_pStatusBuff = 0;
	}
}
//----------------------------------------------------------------
bool SoArrayUID::InitArray(int nElementSize, int nInitCapacity, int nAddCount)
{
	m_nUsingElementCount = 0;
	m_nElementSize = nElementSize;
	m_nAddCount = nAddCount;
	return ResizeArray(nInitCapacity);
}
//----------------------------------------------------------------
bool SoArrayUID::ResizeArray(int newCapacity)
{
	if (newCapacity <= m_nCapacity)
	{
		return true;
	}
	//申请新的内存，包含m_pStatusBuff内存块。
	const int newSize = m_nElementSize * newCapacity + newCapacity;
	char* newElementBuff = (char*)malloc(newSize);
	if (newElementBuff == 0)
	{
		return false;
	}
	char* newStatusBuff = newElementBuff + m_nElementSize * newCapacity;
	for (int i = 0; i < newCapacity; ++i)
	{
		newStatusBuff[i] = Status_Empty;
	}
	//把旧数组的元素内容填充到新数组中，并释放旧数组
	if (m_pElementBuff)
	{
		if (m_nUsingElementCount > 0)
		{
			//注意，这里必须拷贝 m_nCapacity 个元素。
			memcpy(newElementBuff, m_pElementBuff, m_nElementSize * m_nCapacity);
			memcpy(newStatusBuff, m_pStatusBuff, m_nCapacity);
		}
		free(m_pElementBuff);
		m_pElementBuff = 0;
		m_pStatusBuff = 0;
	}
	//把新数组维护起来
	m_pElementBuff = newElementBuff;
	m_pStatusBuff = newStatusBuff;
	m_nCapacity = newCapacity;
	return true;
}
//----------------------------------------------------------------
void SoArrayUID::ClearArray()
{
	if (m_pStatusBuff)
	{
		for (int i = 0; i < m_nCapacity; ++i)
		{
			m_pStatusBuff[i] = Status_Empty;
		}
	}
	m_nUsingElementCount = 0;
}
//----------------------------------------------------------------
int SoArrayUID::FillAt(int nUID, const void* pElement)
{
	if (m_pElementBuff == 0)
	{
		return -1;
	}
	if (pElement == 0)
	{
		return -1;
	}
	if (nUID == -1)
	{
		nUID = FindFirstEmptyElement();
		if (nUID == -1)
		{
			if (ResizeArray(m_nCapacity + m_nAddCount) == false)
			{
				return -1;
			}
			nUID = m_nCapacity - m_nAddCount;
		}
	}
	if (nUID < 0 || nUID >= m_nCapacity)
	{
		return -1;
	}
	//
	SoTinyMemCpy(m_pElementBuff + nUID * m_nElementSize, pElement, m_nElementSize);
	m_pStatusBuff[nUID] = Status_Using;
	++m_nUsingElementCount;
	return nUID;
}
//----------------------------------------------------------------
int SoArrayUID::TakeNew(void** ppElement)
{
	if (m_pElementBuff == 0)
	{
		return -1;
	}
	if (ppElement == 0)
	{
		return -1;
	}
	//
	int nUID = FindFirstEmptyElement();
	if (nUID == -1)
	{
		if (ResizeArray(m_nCapacity + m_nAddCount) == false)
		{
			return -1;
		}
		nUID = m_nCapacity - m_nAddCount;
	}
	if (nUID < 0 || nUID >= m_nCapacity)
	{
		return -1;
	}
	//
	*ppElement = m_pElementBuff + nUID * m_nElementSize;
	m_pStatusBuff[nUID] = Status_Using;
	++m_nUsingElementCount;
	return nUID;
}
//----------------------------------------------------------------
void SoArrayUID::ClearAt(int nUID)
{
	if (m_pStatusBuff == 0)
	{
		return;
	}
	if (nUID < 0 || nUID >= m_nCapacity)
	{
		return;
	}
	//
	if (m_pStatusBuff[nUID] == Status_Using)
	{
		--m_nUsingElementCount;
	}
	m_pStatusBuff[nUID] = (char)Status_Empty;
}
//----------------------------------------------------------------
void* SoArrayUID::GetAt(int nUID) const
{
	if (m_pElementBuff != 0 && nUID >= 0 && nUID < m_nCapacity && m_pStatusBuff[nUID] == Status_Using)
	{
		return (m_pElementBuff + nUID * m_nElementSize);
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
int SoArrayUID::GetStatus(int nUID) const
{
	if (m_pStatusBuff != 0 && nUID >= 0 && nUID < m_nCapacity)
	{
		return (int)(m_pStatusBuff[nUID]);
	}
	else
	{
		return Status_Invalid;
	}
}
//----------------------------------------------------------------
int SoArrayUID::FindFirstEmptyElement()
{
	if (m_pStatusBuff == 0)
	{
		return -1;
	}
	//
	int theUID = -1;
	for (int i = 0; i < m_nCapacity; ++i)
	{
		if (m_pStatusBuff[i] == Status_Empty)
		{
			theUID = i;
			break;
		}
	}
	return theUID;
}
//----------------------------------------------------------------
