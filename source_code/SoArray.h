//----------------------------------------------------------------
#ifndef _SoArray_h_
#define _SoArray_h_
//----------------------------------------------------------------
class SoArray
{
public:
	SoArray();
	~SoArray();
	//重置为初始状态。没有分配任何内存空间的状态。
	void ResetInit();
	//初始化函数。也可以作为清零重置函数，把一个旧数组重置成新数组。
	//--nElementSize 元素大小，即对元素执行sizeof运算得到的返回值。
	//--nInitCapacity 初始时的最大元素个数，也即预分配的内存能够容纳多少个元素。
	//--nAddCount 当数组元素已满，预分配更大内存时，新增加的内存能够容纳多少个元素。
	bool InitArray(int nElementSize, int nInitCapacity, int nAddCount);
	//申请更大的内存，存储更多的元素。
	//如果newCapacity比当前的m_nCapacity小，则什么都不做。
	bool ResizeArray(int newCapacity);
	//清空容器。
	void ClearArray();
	//把新元素添加到队尾。
	//返回值为新元素的位置索引。
	int PushBack(const void* pElement);
	//申请一个新元素，由外界来填充新元素的值。
	//返回值为新元素的位置索引。
	int TakeNew(void** ppElement);
	//把新元素插入到nIndex位置，原nIndex位置的元素以及nIndex位置后面的元素，都顺次后移一个位置。
	//如果nIndex为无效值的话，则会返回修正之后的位置索引。
	//返回值为新元素的位置索引。
	int InsertAt(int nIndex, const void* pElement);
	//移除某个元素。
	bool RemoveAt(int nIndex);
	bool Remove(const void* pElement);
	//两个元素交换位置。
	bool SwapAt(int nIndexA, int nIndexB);
	//获取某个元素。
	void* GetAt(int nIndex) const;
	//获取有效元素的个数。
	int GetSize() const;
	//获取容器的大小。
	int GetCapacity() const;
	//获取某个元素的位置索引。
	int GetIndex(const void* pElement) const;

private:
	//在m_nDeltaAddRemove优化策略下，本函数主要用来寻找空洞。
	int FindEmptyElement() const;

private:
	typedef int arrayuid;

private:
	//存储元素对象。该数组的下标序号就是UID。
	//列表内元素的分布不是连贯的，例如，0号元素为有效值，1号元素为无效值，2号元素为有效值，等等。
	char* m_pElementBuff;
	//以连续的内存空间来存储UID，目的是能够方便的遍历有效元素。
	//列表内元素的分布是连贯的，序号为[0, m_nSize)的元素都是有效值。
	arrayuid* m_pUIDBuff;
	int m_nCapacity;
	int m_nSize;
	int m_nElementSize;
	int m_nAddCount;
	//这是为了对添加新元素操作做优化，如果没有执行过Remove操作，那么添加新元素时，新元素的位置索引必定是m_nSize。
	//删除元素出现空洞，添加元素后空洞被填补上，一次删除和一次添加就相当于没有发生空洞，
	//删除元素就执行--delta，添加元素时如果delta小于0就执行++delta，只要delta值为0，则新元素的位置索引必定是m_nSize，
	//可能出现这样的问题，初始时有效元素的位置索引为[0, m_nSize-1]，先把(m_nSize-2)号元素删除，再把(m_nSize-1)号元素删除，
	//剩下m_nSize-2个元素，有效元素的位置索引是[0, m_nSize-3]，此时没有空洞，但是delta的值为-2。
	//这种情况虽然不正常，但是没有坏的影响，最差就是优化效果暂时不起作用。
	int m_nDeltaAddRemove;
};
//----------------------------------------------------------------
inline void SoArray::ClearArray()
{
	m_nSize = 0;
	m_nDeltaAddRemove = 0;
}
//----------------------------------------------------------------
inline int SoArray::GetSize() const
{
	return m_nSize;
}
//----------------------------------------------------------------
inline int SoArray::GetCapacity() const
{
	return m_nCapacity;
}
//----------------------------------------------------------------
#endif //_SoArray_h_
//----------------------------------------------------------------
