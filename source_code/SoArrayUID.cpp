//----------------------------------------------------------------
#include "SoArrayUID.h"
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
bool SoArrayUID::InitArray(soint32 nElementSize, soint32 nInitCapacity, soint32 nAddCount)
{
	m_nUsingElementCount = 0;
	m_nElementSize = nElementSize;
	m_nAddCount = nAddCount;
	return ResizeArray(nInitCapacity);
}
//----------------------------------------------------------------
bool SoArrayUID::ResizeArray(soint32 newCapacity)
{
	if (newCapacity <= m_nCapacity)
	{
		return true;
	}
	//申请新的内存，包含m_pStatusBuff内存块。
	const soint32 newSize = m_nElementSize * newCapacity + newCapacity;
	char* newElementBuff = (char*)malloc(newSize);
	if (newElementBuff == 0)
	{
		return false;
	}
	char* newStatusBuff = newElementBuff + m_nElementSize * newCapacity;
	for (soint32 i = 0; i < newCapacity; ++i)
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
		for (soint32 i = 0; i < m_nCapacity; ++i)
		{
			m_pStatusBuff[i] = Status_Empty;
		}
	}
	m_nUsingElementCount = 0;
}
//----------------------------------------------------------------
soint32 SoArrayUID::FillAt(soint32 nUID, const void* pElement)
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
	memcpy(m_pElementBuff + nUID * m_nElementSize, pElement, m_nElementSize);
	m_pStatusBuff[nUID] = Status_Using;
	++m_nUsingElementCount;
	return nUID;
}
//----------------------------------------------------------------
soint32 SoArrayUID::TakeNew(void** ppElement)
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
	soint32 nUID = FindFirstEmptyElement();
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
void SoArrayUID::ClearAt(soint32 nUID)
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
void* SoArrayUID::GetAt(soint32 nUID) const
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
////----------------------------------------------------------------
//soint32 SoArrayUID::GetStatus(soint32 nUID) const
//{
//	if (m_pStatusBuff != 0 && nUID >= 0 && nUID < m_nCapacity)
//	{
//		return (soint32)(m_pStatusBuff[nUID]);
//	}
//	else
//	{
//		return Status_Invalid;
//	}
//}
//----------------------------------------------------------------
soint32 SoArrayUID::GetUID(const void* pElement, soint32 nValidSize) const
{
	if (pElement == 0)
	{
		return -1;
	}
	if (m_pElementBuff == 0)
	{
		return -1;
	}
	if (nValidSize > m_nElementSize)
	{
		return -1;
	}
	//
	soint32 theUID = -1;
	const char* destElement = (const char*)pElement;
	char* tempElement = 0;
	bool bFindElement = true;
	for (soint32 i = 0; i < m_nCapacity; ++i)
	{
		if (m_pStatusBuff[i] != Status_Using)
		{
			continue;
		}

		tempElement = m_pElementBuff + i * m_nElementSize;
		bFindElement = true;
		for (soint32 k = 0; k < nValidSize; ++k)
		{
			if (tempElement[k] != destElement[k])
			{
				bFindElement = false;
				break;
			}
		}
		if (bFindElement == true)
		{
			theUID = i;
			break;
		}
	}
	return theUID;
}
//----------------------------------------------------------------
soint32 SoArrayUID::FindFirstEmptyElement()
{
	if (m_pStatusBuff == 0)
	{
		return -1;
	}
	//
	soint32 theUID = -1;
	for (soint32 i = 0; i < m_nCapacity; ++i)
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
