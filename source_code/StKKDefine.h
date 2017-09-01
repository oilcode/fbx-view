//----------------------------------------------------------------
#ifndef _StKKDefine_h_
#define _StKKDefine_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
#include "SoD3DTexture.h"
//----------------------------------------------------------------
//每个顶点最多受几根骨骼影响。
#define StKK_MaxBonePerVertex 4
//----------------------------------------------------------------
enum StKKVertexType
{
	//无效值
	StKKVertexType_Invalid,
	//顶点结构体包含坐标，法线，UV，骨骼（受哪些骨骼影响以及影响权重）。
	StKKVertexType_Pos_Normal_UV_Bone,
	//顶点结构体包含坐标，法线，UV。
	StKKVertexType_Pos_Normal_UV,
};
//----------------------------------------------------------------
struct StKKVertexStruct_Pos_Normal_UV
{
	int PosIndex;
	int NormalIndex;
	int UVIndex;
};
//----------------------------------------------------------------
struct StKKVertexStruct_Pos_Normal_UV_Bone
{
	int PosIndex;
	int NormalIndex;
	int UVIndex;
	int BoneIndex[StKK_MaxBonePerVertex];
	float BoneWeight[StKK_MaxBonePerVertex];
};
//----------------------------------------------------------------
struct StKKBoneTransform
{
	SoMathMatrix3 Rotate;
	SoMathFloat3 Translate;
};
//----------------------------------------------------------------
struct StKKModelData
{
	ID3D11Buffer* pVertexStructBuffer;
	ID3D11Buffer* pIndexBuffer;
	SoD3DTexture* pVertexValueTexture;
	//顶点结构体类型，值为 StKKVertexType 枚举值。
	int nVertexType;
	int nVertexCount;
	int nIndexCount;
	int nSizeofVertexStruct;
	int nTextureWidth;
	int nTextureHeight;
	//坐标值的个数。
	int nPosCount;
	//法线值的个数。
	int nNormalCount;
	//UV值的个数。
	int nUVCount;
	SoMathFloat3 kPosMinValue;
	SoMathFloat3 kPosMaxValue;

	StKKModelData();
	~StKKModelData();
};
//----------------------------------------------------------------
struct StKKModelAnimation
{
	SoD3DTexture* pAnimationTexture;
	int nAnimID;
	int nTextureWidth;
	int nTextureHeight;
	//动画持续时长，单位毫秒。
	int nTimeLength;
	//一共有多少个动画帧。
	int nKeyFrameCount;
	//一共有多少根骨骼。
	int nBoneCount;

	StKKModelAnimation();
	~StKKModelAnimation();
};
//----------------------------------------------------------------
//                           Kkb
//----------------------------------------------------------------
//Kkb文件格式
//1，StKkbFileHead
//2，StKKVertexStruct 列表，顶点结构体
//3，SoMathFloat3 列表，顶点值
//4，SoMathFloat3 列表，法线值
//5，SoMathFloat2 列表，UV值
//----------------------------------------------------------------
#define StKkbFileFlagSize 5
extern const char* StKkbFileFlagString;
//----------------------------------------------------------------
struct StKkbFileHead
{
	//文件标记。
	char FileFlag[StKkbFileFlagSize];
	//顶点结构体类型，值为 StKKVertexType 枚举值。
	int VertexType;
	//顶点结构体的sizeof。
	int VertexSize;
	//顶点结构体的个数。
	int VertexCount;
	//坐标值的个数。
	int PosCount;
	//法线值的个数。
	int NormalCount;
	//UV值的个数。
	int UVCount;
	//三角形面片的索引的排列方式，也即绕序。
	//下面的值为1，表示使用“123顺序构成三角形”；值为0表示使用“321顺序构成三角形”。
	//顶点索引的个数与顶点结构体的个数相同。
	int TriangleIndex_123;
};
//----------------------------------------------------------------
//                           Kkf
//----------------------------------------------------------------
//Kkf文件格式
//1，StKkfFileHead
//2，StKKBoneTransform 列表
//----------------------------------------------------------------
#define StKkfFileFlagSize 5
extern const char* StKkfFileFlagString;
//----------------------------------------------------------------
struct StKkfFileHead
{
	//文件标记。
	char FileFlag[StKkfFileFlagSize];
	//动画持续时长，单位毫秒。
	int TimeLength;
	//一共有多少个动画帧。
	int KeyFrameCount;
	//一共有多少根骨骼。
	int BoneCount;
};
//----------------------------------------------------------------
#endif //_StKKDefine_h_
//----------------------------------------------------------------
