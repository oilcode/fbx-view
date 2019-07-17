//----------------------------------------------------------------
#ifndef _StFBXDefine_h_
#define _StFBXDefine_h_
//----------------------------------------------------------------
#include "SoMath.h"
#include "SoTinyString.h"
#include "SoBitFlag.h"
#include "StFBXUserDefine.h"
//----------------------------------------------------------------
//Vector3有3个float元素
#define StFBX_Sizeof_Vector3 12
//Color有4个float元素
#define StFBX_Sizeof_Color 16
//UV有2个float元素
#define StFBX_Sizeof_UV 8
//像素Pixel的Sizeof
#define StFBX_Sizeof_Pixel 4
//顶点序号的sizeof值，32位整型。
#define StFBX_Sizeof_VertexIndex 4
//----------------------------------------------------------------
enum StFBXElementType
{
	StFBXElement_Position = 0x00000001,
	StFBXElement_Normal = 0x00000002,
	StFBXElement_Tangent = 0x00000004,
	StFBXElement_Color = 0x00000008,
	StFBXElement_UV1 = 0x00000010,
	StFBXElement_UV2 = 0x00000020,
};
//----------------------------------------------------------------
//存储fbx文件中所有的Mesh的顶点数据。
//在解析fbx文件完毕后，这里面存储的顶点数据就是模型默认Pose的顶点数据。
//顶点结构体个数与顶点索引个数是相同的。
//这就意味着，如果一个顶点被两个相邻三角形共用的话，这个顶点数据就被制作成了两份顶点结构体。
struct StFBXMeshData
{
	char* pVertexBuff;
	int nVertexBuffSize;
	int nVertexCount;
	char* pIndexBuff;
	int nIndexBuffSize;
	int nIndexCount;
	//顶点位置序号到控制点位置序号的映射。
	//该数组的长度就是nVertexCount。
	int* pVertexIndex2ControlPointIndex;
	//顶点结构体中含有哪些成员（法线，贴图坐标等等）。
	SoBitFlag kVertexType;
	//顶点结构体的字节数。
	//我们约定，一个fbx文件中，所有的Mesh的顶点数据格式都相同。
	//不同的mesh中，顶点结构体的成员相同，顶点结构体的字节数也相同。
	int nSizeofVertexData;

	StFBXMeshData();
	~StFBXMeshData();
	void SetVertexType(const SoBitFlag& kType);
	void ReserveVertexCount(const int nVertexCount);
};
//----------------------------------------------------------------
//存储骨骼序号和该骨骼对某个控制点的影响权重。
//BoneIndex就是StFBXBoneGroup中的数组下标。
struct StFBXBoneIndexSkinWeight
{
	SoTinyString kBoneName;
	int nBoneIndex;
	float fSkinWeight;

	StFBXBoneIndexSkinWeight();
};
//----------------------------------------------------------------
//单个控制点。
struct StFBXControlPoint
{
	//顶点坐标。
	SoMathFloat3 kVertex;
	//本控制点受哪些骨骼的影响，以及影响权重。
	StFBXBoneIndexSkinWeight kPairList[StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint];

	StFBXControlPoint();
	void AddBoneNameSkinWeight(const char* szBoneName, float fSkinWeight);
};
//----------------------------------------------------------------
//存储fbx中所有的Mesh的控制点。
struct StFBXBoneGroup; //前向声明
struct StFBXControlPointGroup
{
private:
	StFBXControlPoint* kControlPointArray;
	int nControlPointValidCount;
	int nControlPointMaxCount;

public:
	StFBXControlPointGroup();
	~StFBXControlPointGroup();
	void ReserveControlPointCount(int nCount);
	//向外界返回一个未使用的 StFBXControlPoint 对象，由外界对其赋值。
	StFBXControlPoint* TakeNew();
	StFBXControlPoint* GetAt(int nIndex) const;
	int GetSize() const;
	//最开始，StFBXBoneIndexSkinWeight数据中存储的是骨骼名字。
	//当有了骨骼数据后，就将骨骼名字转换成相应的骨骼序号。
	void MakeBoneIndexByBoneName(const StFBXBoneGroup* pBoneGroup);

	//检查每个控制点的受骨骼影响权重，保证权重的总和是1。
	//我现在认为不需要保证权重的总和是1，
	//例如，有一个顶点，它只受两根骨骼的轻微的影响，
	//权重值比较小才能体现“轻微”，那么权重的总和必定不是1。
	//void CheckBoneSkinWeight();
};
//----------------------------------------------------------------
//骨骼节点
struct StFBXBone
{
	//本骨骼的名字。
	SoTinyString kBoneName;
	//父骨骼在层级列表中的序号位置。
	int nParentIndex;
	//子骨骼在层级列表中的序号位置。每个元素都是一个int值。
	int kChildIndexList[StFBX_MaxCount_ChildBone];
	//动画播放过程中，顶点坐标的计算公式：
	//VertexAtTimeT = VertexFromControlPoint * kMatFromBoneSpaceToWorldSpace * KeyFrameMatrixAtTimeT
	SoMathMatrix4 kMatFromBoneSpaceToModelSpace;

	StFBXBone();
	~StFBXBone();
};
//----------------------------------------------------------------
//存储fbx文件中所有的Bone，骨骼层级结构 Hierarchy。
struct StFBXBoneGroup
{
private:
	StFBXBone* kBoneArray;
	int nBoneValidCount;
	int nBoneMaxCount;

public:
	StFBXBoneGroup();
	~StFBXBoneGroup();
	void ReserveBoneCount(int nCount);
	//遍历骨骼列表，分别生成每个骨骼的子骨骼列表。
	void GenerateChildren();
	int GetBoneIndexByBoneName(const char* szBoneName) const;
	//向外界返回一个未使用的 StFBXBone 对象，由外界对其赋值。
	StFBXBone* TakeNew(int* pIndex);
	StFBXBone* GetAt(int nIndex) const;
	int GetSize() const;
};
//----------------------------------------------------------------
//动画帧
struct StFBXKeyFrame
{
	float fKeyTime;
	SoMathMatrix4 matKeyTransform;

	StFBXKeyFrame();
};
//----------------------------------------------------------------
//在一个动画中，一个Bone所携带的所有的动画帧。
//BoneIndex就是StFBXBoneGroup中的数组下标。
struct StFBXBoneAnimation
{
	StFBXKeyFrame* kKeyFrameArray;
	int nFrameValidCount;
	int nFrameMaxCount;
	int nBoneIndex;

	StFBXBoneAnimation();
	~StFBXBoneAnimation();
	void ReserveKeyFrameCount(int nCount);
	//向外界返回一个未使用的 StFBXKeyFrame 对象，由外界对其赋值。
	StFBXKeyFrame* TakeNew();
	StFBXKeyFrame* GetAt(int nIndex) const;
	int GetSize() const;
};
//----------------------------------------------------------------
//在一个动画中，一个模型所携带的所有的动画帧。
struct StFBXModelAnimation
{
	//所有骨骼的动画帧列表。每个元素都是一个StFBXBoneAnimation对象。
	//一般情况下，本数组的size就是骨骼总个数。
	//有可能出现这种情况，在动画中，只有一部分骨骼发挥了作用，
	//那么本数组的size就会小于骨骼总个数。
	StFBXBoneAnimation* kBoneAnimationArray;
	int nAnimValidCount;
	int nAnimMaxCount;
	//动画有多少帧。
	int nFrameCount;
	//动画的持续时长。单位秒。
	float fAnimLength;

	StFBXModelAnimation();
	~StFBXModelAnimation();
	void ReserveBoneCount(int nBoneCount);
	int GetSize() const;
	int GetFrameCount() const;
	//向外界返回一个未使用的 StFBXBoneAnimation 对象，由外界对其赋值。
	StFBXBoneAnimation* TakeNew();
	StFBXBoneAnimation* GetAt(int nIndex) const;
	StFBXBoneAnimation* GetBoneAnimation(int nBoneIndex) const;
	//根据指定的时间，获取该时间要播放动画的哪一帧。
	int GetKeyFrameIndexByTime(float fTime);
};
//----------------------------------------------------------------
#endif //_StFBXDefine_h_
//----------------------------------------------------------------
