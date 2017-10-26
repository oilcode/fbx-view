//----------------------------------------------------------------
//1，遍历过程中不要添加元素或者删除元素。
//----------------------------------------------------------------
#include "SoArray.h"
//----------------------------------------------------------------
SoArray::SoArray()
:m_pElementBuff(0)
,m_pUIDBuff(0)
,m_nCapacity(0)
,m_nSize(0)
,m_nElementSize(0)
,m_nAddCount(0)
,m_nDeltaAddRemove(0)
{

}
//----------------------------------------------------------------
SoArray::~SoArray()
{
	if (m_pElementBuff)
	{
		free(m_pElementBuff);
		m_pElementBuff = 0;
		m_pUIDBuff = 0;
	}
}
//----------------------------------------------------------------
void SoArray::ResetInit()
{
	m_pElementBuff = 0;
	m_pUIDBuff = 0;
	m_nCapacity = 0;
	m_nSize = 0;
	m_nElementSize = 0;
	m_nAddCount = 0;
	m_nDeltaAddRemove = 0;
}
//----------------------------------------------------------------
bool SoArray::InitArray(soint32 nElementSize, soint32 nInitCapacity, soint32 nAddCount)
{
	m_nSize = 0;
	m_nDeltaAddRemove = 0;
	m_nElementSize = nElementSize;
	m_nAddCount = nAddCount;
	return ResizeArray(nInitCapacity);
}
//----------------------------------------------------------------
bool SoArray::ResizeArray(soint32 newCapacity)
{
	if (newCapacity <= m_nCapacity)
	{
		return true;
	}
	//申请新的内存
	const soint32 newSize = m_nElementSize * newCapacity + sizeof(arrayuid) * newCapacity;
	char* newElementBuff = (char*)malloc(newSize);
	if (newElementBuff == 0)
	{
		return false;
	}
	arrayuid* newIndexBuff = (arrayuid*)(newElementBuff + m_nElementSize * newCapacity);
	//把旧数组的元素内容填充到新数组中，并释放旧数组
	if (m_pElementBuff)
	{
		if (m_nSize > 0)
		{
			//注意，这里必须拷贝 m_nCapacity 个元素，不能是 m_nSize 个元素。
			memcpy(newElementBuff, m_pElementBuff, m_nElementSize * m_nCapacity);
			memcpy(newIndexBuff, m_pUIDBuff, sizeof(arrayuid) * m_nCapacity);
		}
		free(m_pElementBuff);
		m_pElementBuff = 0;
		m_pUIDBuff = 0;
	}
	//把新数组维护起来
	m_pElementBuff = newElementBuff;
	m_pUIDBuff = newIndexBuff;
	m_nCapacity = newCapacity;
	return true;
}
//----------------------------------------------------------------
soint32 SoArray::PushBack(const void* pElement)
{
	if (pElement == 0)
	{
		return -1;
	}
	if (m_pElementBuff == 0)
	{
		return -1;
	}
	if (m_nSize >= m_nCapacity)
	{
		if (ResizeArray(m_nCapacity + m_nAddCount) == false)
		{
			return -1;
		}
	}
	//
	soint32 newUID = m_nSize;
	if (m_nDeltaAddRemove < 0)
	{
		newUID = FindEmptyElement();
		if (newUID == -1)
		{
			return -1;
		}
		++m_nDeltaAddRemove;
	}
	//
	memcpy(m_pElementBuff+newUID*m_nElementSize, pElement, m_nElementSize);
	m_pUIDBuff[m_nSize] = newUID;
	return m_nSize++;
}
//----------------------------------------------------------------
soint32 SoArray::TakeNew(void** ppElement)
{
	if (ppElement == 0)
	{
		return -1;
	}
	if (m_pElementBuff == 0)
	{
		return -1;
	}
	if (m_nSize >= m_nCapacity)
	{
		if (ResizeArray(m_nCapacity + m_nAddCount) == false)
		{
			return -1;
		}
	}
	//
	soint32 newUID = m_nSize;
	if (m_nDeltaAddRemove < 0)
	{
		newUID = FindEmptyElement();
		if (newUID == -1)
		{
			return -1;
		}
		++m_nDeltaAddRemove;
	}
	//
	*ppElement = (m_pElementBuff + newUID * m_nElementSize);
	m_pUIDBuff[m_nSize] = newUID;
	return m_nSize++;
}
//----------------------------------------------------------------
soint32 SoArray::InsertAt(soint32 nIndex, const void* pElement)
{
	if (nIndex < 0)
	{
		nIndex = 0;
	}
	if (nIndex > m_nSize)
	{
		nIndex = m_nSize;
	}
	//
	if (PushBack(pElement) == -1)
	{
		return -1;
	}
	//
	if (nIndex == m_nSize - 1)
	{
		return nIndex;
	}
	//
	arrayuid theUID = m_pUIDBuff[m_nSize-1];
	for (soint32 i = m_nSize-1; i > nIndex; --i)
	{
		m_pUIDBuff[i] = m_pUIDBuff[i - 1];
	}
	m_pUIDBuff[nIndex] = theUID;
	return nIndex;
}
//----------------------------------------------------------------
bool SoArray::RemoveAt(soint32 nIndex)
{
	if (nIndex < 0 || nIndex >= m_nSize)
	{
		return false;
	}
	if (m_pUIDBuff == 0)
	{
		return false;
	}
	//
	for (soint32 i = nIndex + 1; i < m_nSize; ++i)
	{
		m_pUIDBuff[i-1] = m_pUIDBuff[i];
	}
	m_pUIDBuff[m_nSize-1] = -1;
	--m_nSize;
	--m_nDeltaAddRemove;
	return true;
}
//----------------------------------------------------------------
bool SoArray::Remove(const void* pElement)
{
	bool br = false;
	const soint32 nIndex = GetIndex(pElement, m_nElementSize);
	if (nIndex != -1)
	{
		br = RemoveAt(nIndex);
	}
	return br;
}
//----------------------------------------------------------------
bool SoArray::SwapAt(soint32 nIndexA, soint32 nIndexB)
{
	if (nIndexA < 0 || nIndexA >= m_nSize)
	{
		return false;
	}
	if (nIndexB < 0 || nIndexB >= m_nSize)
	{
		return false;
	}
	if (m_pUIDBuff == 0)
	{
		return false;
	}
	if (nIndexA == nIndexB)
	{
		return true;
	}
	//
	const arrayuid nTemp = m_pUIDBuff[nIndexA];
	m_pUIDBuff[nIndexA] = m_pUIDBuff[nIndexB];
	m_pUIDBuff[nIndexB] = nTemp;
	return true;
}
//----------------------------------------------------------------
void* SoArray::GetAt(soint32 nIndex) const
{
	if (m_pElementBuff != 0 && nIndex >= 0 && nIndex < m_nSize)
	{
		const soint32 theUID = m_pUIDBuff[nIndex];
		return (m_pElementBuff + theUID * m_nElementSize);
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
soint32 SoArray::GetIndex(const void* pElement, soint32 nValidSize) const
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
	soint32 theIndex = -1;
	const char* destElement = (const char*)pElement;
	char* tempElement = 0;
	bool bFindElement = true;
	for (soint32 i = 0; i < m_nSize; ++i)
	{
		const soint32 theUID = m_pUIDBuff[i];
		tempElement = m_pElementBuff + theUID * m_nElementSize;
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
			theIndex = i;
			break;
		}
	}
	return theIndex;
}
//----------------------------------------------------------------
soint32 SoArray::FindEmptyElement() const
{
	soint32 theUID = -1;
	const soint32 nCurrentSize = m_nSize;
	const soint32 newSize = nCurrentSize + 1;
	bool bElementExist = false;
	//倒序查找，能够更快的找到空元素。
	//for (soint32 i = newSize-1; i >= 0; --i)
	//正序查找，用来寻找空洞。
	for (soint32 i = 0; i < newSize; ++i)
	{
		bElementExist = false;
		for (soint32 k = 0; k < nCurrentSize; ++k)
		{
			if (m_pUIDBuff[k] == i)
			{
				bElementExist = true;
				break;
			}
		}
		if (bElementExist == false)
		{
			theUID = i;
			break;
		}
	}
	return theUID;
}
//----------------------------------------------------------------
