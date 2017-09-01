//----------------------------------------------------------------
#ifndef _SoArrayUID_h_
#define _SoArrayUID_h_
//----------------------------------------------------------------
class SoArrayUID
{
public:
	enum eStatusType
	{
		Status_Invalid = -1,
		Status_Empty, //元素值为空
		Status_Using, //元素值是有效值，正在被外界使用
	};

public:
	SoArrayUID();
	~SoArrayUID();
	//初始化函数。也可以作为清零重置函数，把一个旧数组重置成新数组。
	//--nElementSize 元素大小，即对元素执行sizeof运算得到的返回值。
	//--nInitCapacity 初始时的最大元素个数，也即预分配的内存能够容纳多少个元素。
	//--nAddCount 当数组元素已满，预分配更大内存时，新增加的内存能够容纳多少个元素。
	bool InitArray(int nElementSize, int nInitCapacity, int nAddCount);
	//申请更大的内存，存储更多的元素。
	//如果newCapacity比当前的m_nCapacity小，则什么都不做。
	bool ResizeArray(int newCapacity);
	//清除所有的元素。内存不会被释放掉。
	void ClearArray();
	//把新元素填充到nUID位置，会把原nUID位置的元素覆盖掉。
	//返回值为新元素的UID。
	//nUID的值为-1，表示自动找一个Status_Empty位置进行填充。
	int FillAt(int nUID, const void* pElement);
	//申请一个新元素，由外界来填充新元素的值。
	//必须找到状态为Status_Empty的元素，然后再返回。
	//返回值为新元素的位置索引。
	int TakeNew(void** ppElement);
	//清除nUID位置的元素，把其状态设置成Status_Empty。
	void ClearAt(int nUID);
	//获取容器的容量。
	int GetCapacity() const;
	//获取nUID位置的元素，并且该元素的状态必须是Status_Using；否则返回NULL。
	void* GetAt(int nUID) const;
	//状态操作。
	int GetStatus(int nUID) const;
	//获取有效元素的个数。
	int GetUsingElementCount() const;

private:
	//找到第一个空元素。
	int FindFirstEmptyElement();

private:
	//存储元素对象。该数组的下标序号就是UID。
	//列表内元素的分布不是连贯的，例如，0号元素为有效值，1号元素为无效值，2号元素为有效值，等等。
	char* m_pElementBuff;
	//存储元素状态，其值为eStatusType枚举值。
	char* m_pStatusBuff;
	int m_nCapacity;
	int m_nUsingElementCount;
	int m_nElementSize;
	int m_nAddCount;
};
//----------------------------------------------------------------
inline int SoArrayUID::GetCapacity() const
{
	return m_nCapacity;
}
//----------------------------------------------------------------
inline int SoArrayUID::GetUsingElementCount() const
{
	return m_nUsingElementCount;
}
//----------------------------------------------------------------
#endif //_SoArrayUID_h_
//----------------------------------------------------------------
