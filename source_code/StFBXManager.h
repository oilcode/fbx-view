//----------------------------------------------------------------
#ifndef _StFBXManager_h_
#define _StFBXManager_h_
//----------------------------------------------------------------
#include "fbxsdk.h"
#include "SoMath.h"
#include "SoBitFlag.h"
//----------------------------------------------------------------
struct StFBXMeshData;
struct StFBXControlPointGroup;
struct StFBXBoneGroup;
struct StFBXModelAnimation;
struct StFBXBoneAnimation;
class StFBXModel;
//----------------------------------------------------------------
class StFBXManager
{
public:
	static bool CreateFBXManager();
	static void ReleaseFBXManager();
	static StFBXManager* Get();

	bool LoadFBX(const char* szFileName, StFBXModel* pFBXModel);

private:
	StFBXManager();
	~StFBXManager();
	bool InitFBXManager();
	void ClearFBXManager();


	//遍历每个Mesh，检查所有的Mesh是否含有相同的顶点结构，例如都含有法线数据，都含有两套UV数据。
	//同时也检查每个Mesh的MappingMode值，目前只支持 EMappingMode::eByPolygonVertex 值。
	//同时计算所有的Mesh的总顶点个数。
	void ParseMeshVertexTotalCountAndVertexType(FbxNode* pNode, SoBitFlag* pVertexType, int* pTotalCount, bool* pAllMeshOK);
	void ParseSingleMeshVertexType(FbxMesh* pFbxMesh, SoBitFlag* pVertexType, bool* pAllMeshOK, bool bFirstMesh);


	//递归遍历节点树，获取所有Mesh的顶点数据。
	//--pProcessedVertexCount 已经处理了多少个顶点数据，也即下一个Mesh将从哪一个顶点位置开始填充。
	//--pProcessedControlPointCount 已经处理了多少个控制点，用于计算下一个Mesh的控制点的位置序号偏移量。
	void ParseMeshData(FbxNode* pNode, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount);
	void LoadSingleMesh(FbxMesh* pFbxMesh, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount);
	void LoadSingleMesh_Normal(FbxMesh* pFbxMesh, int nVertexIndex, FbxVector4* pNormal);
	void LoadSingleMesh_Tangent(FbxMesh* pFbxMesh, int nVertexIndex, FbxVector4* pTangent);
	void LoadSingleMesh_Color(FbxMesh* pFbxMesh, int nVertexIndex, FbxColor* pColor);
	void LoadSingleMesh_UV(FbxMesh* pFbxMesh, int nUVIndex, int nVertexIndex, FbxVector2* pUV);


	//递归遍历节点树，计算所有Mesh中的ControlPoint总个数。
	void ParseControlPointTotalCount(FbxNode* pNode, int* pTotalCount);
	//递归遍历节点树，获取所有Mesh的ControlPoint。
	void ParseAllControlPoint(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup);
	void LoadControlPointFromMesh(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup);
	//递归遍历节点树，获取所有的ControlPoint的蒙皮数据。
	//--pProcessedControlPointCount 已经处理了多少个控制点，用于计算下一个Mesh的控制点的位置序号偏移量。
	void ParseAllControlPointSkinWeight(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup, int* pProcessedControlPointCount);
	void LoadSingleControlPointSkinWeight(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup, int* pProcessedControlPointCount);


	//递归遍历节点树，计算Bone的总个数。
	void ParseBoneTotalCount(FbxNode* pNode, int* pTotalCount);
	//递归遍历节点树，获取所有的Bone节点。
	//--nParentIndex 解析一个Node时，要把它的父骨骼的序号传递给该Node。
	void ParseBoneHierarchy(FbxNode* pNode, StFBXBoneGroup* pGroup, int nParentIndex);
	//递归遍历节点树，计算每个Bone从骨骼节点空间转换到模型坐标系（空间）所需要的矩阵
	void ParseAllBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, FbxNode* pRootNode, StFBXBoneGroup* pBoneGroup);
	void LoadSingleBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, FbxMesh* pMesh, FbxNode* pRootNode, StFBXBoneGroup* pBoneGroup);


	//递归遍历节点树，提取动画帧数据。
	void ParseAllBoneAnimationData(FbxScene* pSDKScene, StFBXBoneGroup* pBoneGroup, StFBXModelAnimation* pModelAnim);


	//计算节点的GeometryTransformation。
	void GetGeometryTransformation(FbxNode* pNode, FbxAMatrix* pMat);
	//转换Matrix对象。
	void ConvertFbxAMatrixToSoMathMatrix4(const FbxAMatrix* pFbxAMatrix, SoMathMatrix4* pSoMatrix);
	//根据BoneIndex和BoneName查找指定的骨骼。
	void FindBoneByIndexAndName(FbxNode* pNode, int nBoneIndex, const char* szBoneName, FbxNode** ppResultNode, int* pAccBoneCount);

private:
	static StFBXManager* ms_pInstance;
	FbxManager* m_pSDKManager;
};
//----------------------------------------------------------------
inline StFBXManager* StFBXManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_StFBXManager_h_
//----------------------------------------------------------------
