//----------------------------------------------------------------
#ifndef _StFBXManager_h_
#define _StFBXManager_h_
//----------------------------------------------------------------
#include "fbxsdk.h"
#include "SoMath.h"
//----------------------------------------------------------------
struct StFBXMeshData;
struct StFBXControlPointGroup;
struct StFBXBoneGroup;
struct StFBXModelAnimation;
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


	//获取所有Mesh的顶点的总个数，并解析一个Mesh中包含哪些数据（法线，贴图UV，顶点色等等）。
	//我们约定，一个fbx文件中，所有的Mesh的顶点数据格式都相同，也即顶点数据中包含的成员都相同。
	//所以，函数内部只需解析一个Mesh对象即可。
	void ParveMeshVertexTotalCountAndVertexType(FbxNode* pNode, int* pTotalCount, StFBXMeshData* pMeshData);
	//解析一个Mesh中包含哪些数据（法线，贴图UV，顶点色等等）。顶点数据中有哪些成员。
	void ParseVertexType(FbxMesh* pFbxMesh, StFBXMeshData* pMeshData);
	//递归遍历节点树，获取所有Mesh的顶点数据。
	//--pProcessedVertexCount 已经处理了多少个顶点数据，也即下一个Mesh将从哪一个顶点位置开始填充。
	//--pProcessedControlPointCount 已经处理了多少个控制点，用于计算下一个Mesh的控制点的位置序号偏移量。
	void ParseMeshData(FbxNode* pNode, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount);
	void LoadSingleMesh(FbxMesh* pFbxMesh, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount);
	void LoadSingleMesh_Normal(FbxMesh* pFbxMesh, int nControlPointIndex, int nAccVertexCount, FbxVector4* pNormal);
	void LoadSingleMesh_Tangent(FbxMesh* pFbxMesh, int nControlPointIndex, int nAccVertexCount, FbxVector4* pTangent);
	void LoadSingleMesh_Color(FbxMesh* pFbxMesh, int nControlPointIndex, int nAccVertexCount, FbxColor* pColor);
	void LoadSingleMesh_UV(FbxMesh* pFbxMesh, int nUVIndex, int nControlPointIndex, int nAccVertexCount, FbxVector2* pUV);


	//递归遍历节点树，计算所有Mesh中的ControlPoint总个数。
	void ParseControlPointTotalCount(FbxNode* pNode, int* pTotalCount);
	//递归遍历节点树，获取所有Mesh的ControlPoint。
	void ParseAllControlPoint(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup);
	void LoadControlPointFromMesh(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup);


	//递归遍历节点树，计算Bone的总个数。
	void ParseBoneTotalCount(FbxNode* pNode, int* pTotalCount);
	//递归遍历节点树，获取所有的Bone节点。
	//--nParentIndex 解析一个Node时，要把它的父骨骼的序号传递给该Node。
	void ParseBoneHierarchy(FbxNode* pNode, StFBXBoneGroup* pGroup, int nParentIndex);


	//递归遍历节点树，获取所有Bone的对控制点的影响权重
	//--pProcessedControlPointCount 已经处理了多少个控制点，用于计算下一个Mesh的控制点的位置序号偏移量。
	void ParseAllBoneSkinWeight(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup, StFBXBoneGroup* pBoneGroup, int* pProcessedControlPointCount);
	void LoadSingleBoneSkinWeight(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup, StFBXBoneGroup* pBoneGroup, int* pProcessedControlPointCount);


	//递归遍历节点树，计算每个Bone从骨骼节点空间转换到模型坐标系（空间）所需要的矩阵
	void ParseAllBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, StFBXBoneGroup* pBoneGroup);
	void LoadSingleBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, FbxMesh* pMesh, StFBXBoneGroup* pBoneGroup);


	//递归遍历节点树，获取每个Bone的动画数据。
	void ParseAllBoneAnimationData(FbxNode* pNode, FbxTime* pBeginTime, FbxTime* pEndTime, StFBXBoneGroup* pBoneGroup, StFBXModelAnimation* pModelAnim);
	void LoadSingleBoneAnimationData(FbxNode* pNode, FbxMesh* pMesh, FbxTime* pBeginTime, FbxTime* pEndTime, StFBXBoneGroup* pBoneGroup, StFBXModelAnimation* pModelAnim);


	//计算节点的GeometryTransformation。
	void GetGeometryTransformation(FbxNode* pNode, FbxAMatrix* pMat);
	void ConvertFbxAMatrixToSoMathMatrix4(FbxAMatrix* pFbxAMatrix, SoMathMatrix4* pSoMatrix);
	//获取动画数据的起止时间。
	void GetAnimationTimeLength(FbxScene* pSDKScene, FbxTime* pBeginTime, FbxTime* pEndTime);

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
