//----------------------------------------------------------------
#ifndef _SoArrayUID_h_
#define _SoArrayUID_h_
//----------------------------------------------------------------
#include "SoCodeBaseInclude.h"
//----------------------------------------------------------------
class SODLL SoArrayUID
{
public:
	SoArrayUID();
	~SoArrayUID();
	//初始化函数。也可以作为清零重置函数，把一个旧数组重置成新数组。
	//--nElementSize 元素大小，即对元素执行sizeof运算得到的返回值。
	//--nInitCapacity 初始时的最大元素个数，也即预分配的内存能够容纳多少个元素。
	//--nAddCount 当数组元素已满，预分配更大内存时，新增加的内存能够容纳多少个元素。
	bool InitArray(soint32 nElementSize, soint32 nInitCapacity, soint32 nAddCount);
	//申请更大的内存，存储更多的元素。
	//如果newCapacity比当前的m_nCapacity小，则什么都不做。
	bool ResizeArray(soint32 newCapacity);
	//清除所有的元素。内存不会被释放掉。
	void ClearArray();
	//把新元素填充到nUID位置，会把原nUID位置的元素覆盖掉。
	//返回值为新元素的UID。
	//nUID的值为-1，表示自动找一个Status_Empty位置进行填充。
	soint32 FillAt(soint32 nUID, const void* pElement);
	//申请一个新元素，由外界来填充新元素的值。
	//必须找到状态为Status_Empty的元素，然后再返回。
	//返回值为新元素的位置索引。
	soint32 TakeNew(void** ppElement);
	//清除nUID位置的元素，把其状态设置成Status_Empty。
	void ClearAt(soint32 nUID);
	//获取容器的容量。
	soint32 GetCapacity() const;
	//获取nUID位置的元素，并且该元素的状态必须是Status_Using；否则返回NULL。
	void* GetAt(soint32 nUID) const;
	////状态操作。
	//soint32 GetStatus(soint32 nUID) const;
	//获取有效元素的个数。
	soint32 GetUsingElementCount() const;
	//获取某个元素的UID。
	//--nValidSize 元素的有效size，也即，判断多少个字节就可以得出“元素相同”或者“元素不相同”的结论。
	//             如果该元素是字符串，则nValidSize就是字符串的长度。
	//如果元素是字符串，则填写字符串的长度就非常重要。例如：
	//本数组的元素是字符串，每个元素的size是5。
	//添加一个元素 "ab0cd" ，字符串实际值是 "ab" ，但是填充之后元素的内存的值是 "ab0cd" 。
	//当查找一个元素 "ab0ff" 时，你认为查找的字符串是 "ab" ，肯定能找到，但是实际查找的元素是 "ab0ff" ，结果是找不到。
	soint32 GetUID(const void* pElement, soint32 nValidSize) const;
    //获取元素的大小。
    soint32 GetSizeofElement() const;

private:
	//找到第一个空元素。
	soint32 FindFirstEmptyElement();

private:
	enum eStatusType
	{
		Status_Invalid = 0,
		Status_Empty = 1, //元素值为空
		Status_Using = 2, //元素值是有效值，正在被外界使用
	};

private:
	//存储元素对象。该数组的下标序号就是UID。
	//列表内元素的分布不是连贯的，例如，0号元素为有效值，1号元素为无效值，2号元素为有效值，等等。
	char* m_pElementBuff;
	//存储元素状态，其值为eStatusType枚举值。
	char* m_pStatusBuff;
	soint32 m_nCapacity;
	soint32 m_nUsingElementCount;
	soint32 m_nElementSize;
	soint32 m_nAddCount;
};
//----------------------------------------------------------------
inline soint32 SoArrayUID::GetCapacity() const
{
	return m_nCapacity;
}
//----------------------------------------------------------------
inline soint32 SoArrayUID::GetUsingElementCount() const
{
	return m_nUsingElementCount;
}
//----------------------------------------------------------------
inline soint32 SoArrayUID::GetSizeofElement() const
{
    return m_nElementSize;
}
//----------------------------------------------------------------
#endif //_SoArrayUID_h_
//----------------------------------------------------------------
