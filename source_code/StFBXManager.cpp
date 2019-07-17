//----------------------------------------------------------------
#include "StFBXManager.h"
#include "StFBXModel.h"
#include "SoSimpleLog.h"
#include "SoStringHelp.h"
#include <Windows.h>
//----------------------------------------------------------------
StFBXManager* StFBXManager::ms_pInstance = 0;
//----------------------------------------------------------------
bool StFBXManager::CreateFBXManager()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new StFBXManager;
		if (ms_pInstance && ms_pInstance->InitFBXManager())
		{
			br = true;
		}
		else
		{
			ReleaseFBXManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void StFBXManager::ReleaseFBXManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
StFBXManager::StFBXManager()
:m_pSDKManager(0)
{

}
//----------------------------------------------------------------
StFBXManager::~StFBXManager()
{
	ClearFBXManager();
}
//----------------------------------------------------------------
bool StFBXManager::InitFBXManager()
{
	m_pSDKManager = FbxManager::Create();
	if (m_pSDKManager == 0)
	{
		SoLogError("StFBXManager::InitFBXManager : FbxManager::Create() fail");
		return false;
	}

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(m_pSDKManager, IOSROOT);
	m_pSDKManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	m_pSDKManager->LoadPluginsDirectory(lPath.Buffer());

	SoLogDebug("StFBXManager::InitFBXManager : success");
	return true;
}
//----------------------------------------------------------------
void StFBXManager::ClearFBXManager()
{
	SoLogDebug("StFBXManager::ClearFBXManager : start");
	if (m_pSDKManager)
	{
		m_pSDKManager->Destroy();
		m_pSDKManager = 0;
	}
}
//----------------------------------------------------------------
bool StFBXManager::LoadFBX(const char* szFileName, StFBXModel* pFBXModel)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		SoLogError("StFBXManager::LoadFBX : szFileName == 0 || szFileName[0] == 0");
		return false;
	}
	if (pFBXModel == 0)
	{
		SoLogError("StFBXManager::LoadFBX : pFBXModel == 0");
		return false;
	}

	bool br = false;
	FbxScene* pSDKScene = 0;
	FbxImporter* pSDKImporter = 0;
	SoLogDebug("StFBXManager::LoadFBX : Start; FileName[%s]", szFileName);

	do 
	{
		pSDKScene = FbxScene::Create(m_pSDKManager, "");
		if (pSDKScene == 0)
		{
			SoLogError("StFBXManager::LoadFBX : FbxScene::Create() fail");
			break;
		}

		pSDKImporter = FbxImporter::Create(m_pSDKManager, "");
		if (pSDKImporter == 0)
		{
			SoLogError("StFBXManager::LoadFBX : FbxImporter::Create() fail");
			break;
		}

		if (pSDKImporter->Initialize(szFileName, -1, m_pSDKManager->GetIOSettings()) == false)
		{
			SoLogError("StFBXManager::LoadFBX : m_pSDKImporter->Initialize fail");
			break;
		}

		if (pSDKImporter->Import(pSDKScene) == false)
		{
			SoLogError("StFBXManager::LoadFBX : m_pSDKImporter->Import fail");
			break;
		}

		//我们使用与3Dmax软件相同的坐标系
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eMax);
		FbxAxisSystem SceneAxisSystem = pSDKScene->GetGlobalSettings().GetAxisSystem();
		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(pSDKScene);
		}

		// Convert Unit System to what is used in this example, if needed
		FbxSystemUnit SceneSystemUnit = pSDKScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			//The unit in this example is centimeter.
			FbxSystemUnit::cm.ConvertScene(pSDKScene);
		}

		FbxGeometryConverter lGeomConverter(m_pSDKManager);
		//FBX文件支持Nurbes，Polygon，Triangle等网格类型，
		//这个函数用来把非Triangle网格转换成Triangle网格类型。
		//如果你确信FBX文件中只含有Triangle网格，那么可以跳过本函数。
		lGeomConverter.Triangulate(pSDKScene, true);
		//Split meshes per material, so that we only have one material per mesh (for VBO support)
		//This function will look through your scene and for each material, it will split the meshes and in turn split the uv's.
		//一个MeshA可能使用了多个material，例如前100个顶点使用了materialA，后100个顶点使用了materialB。
		//这个函数把MeshA切分成了2个Mesh，一个Mesh使用materialA，另一个Mesh使用materialB。
		lGeomConverter.SplitMeshesPerMaterial(pSDKScene, true);

		SoLogDebug("StFBXManager::LoadFBX : Load file success");

		//开始解析
		FbxNode* pSceneRootNode = pSDKScene->GetRootNode();
		StFBXMeshData* pMeshData = pFBXModel->GetMeshData_Modify();
		StFBXControlPointGroup* pControlPointGroup = pFBXModel->GetControlPointGroup_Modify();
		StFBXBoneGroup* pBoneGroup = pFBXModel->GetBoneGroup_Modify();
		StFBXModelAnimation* pModelAnimation = pFBXModel->GetAnimation_Modify();

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//遍历每个Mesh，检查所有的Mesh是否含有相同的顶点结构，例如都含有法线数据，都含有两套UV数据。
		//同时也检查每个Mesh的MappingMode值，目前只支持 EMappingMode::eByPolygonVertex 值。
		//同时计算所有的Mesh的总顶点个数。
		SoBitFlag kVertexType;
		int nMeshVertexTotalCount = 0;
		bool bAllMeshOK = true;
		ParseMeshVertexTotalCountAndVertexType(pSceneRootNode, &kVertexType, &nMeshVertexTotalCount, &bAllMeshOK);
		if (bAllMeshOK == false)
		{
			::MessageBoxA(NULL, "Mesh含有不同的顶点结构，或者某个Mesh的MappingMode值不为eByPolygonVertex！详见log文件", "FBX File Error", MB_OK);
			SoLogError("StFBXManager::LoadFBX : ParseMeshVertexTotalCountAndVertexType fail");
			break;
		}
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//如果nMeshVertexTotalCount值为0，说明fbx文件内没有Mesh，那么就不需要解析顶点数据，也不需要解析骨骼数据。
		//如果nMeshVertexTotalCount值为0，说明fbx文件可能只包含动画数据。
		if (nMeshVertexTotalCount > 0)
		{
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//生成Mesh顶点数据。
			//注意，先调用SetVertexType再调用ReserveVertexCount。
			pMeshData->SetVertexType(kVertexType);
			pMeshData->ReserveVertexCount(nMeshVertexTotalCount);
			int nProcessedVertexCount = 0;
			int nProcessedControlPointCount = 0;
			ParseMeshData(pSceneRootNode, pMeshData, &nProcessedVertexCount, &nProcessedControlPointCount);
			SoLogDebug("StFBXManager::LoadFBX : VertexCount[%d] VertexType[%u] VertexStructSize[%d]", pMeshData->nVertexCount, pMeshData->kVertexType.GetValue(), pMeshData->nSizeofVertexData);
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//生成控制点数据。
			int nControlPointTotalCount = 0;
			ParseControlPointTotalCount(pSceneRootNode, &nControlPointTotalCount);
			pControlPointGroup->ReserveControlPointCount(nControlPointTotalCount);
			ParseAllControlPoint(pSceneRootNode, pControlPointGroup);
			SoLogDebug("StFBXManager::LoadFBX : ControlPointTotalCount[%d]", pControlPointGroup->GetSize());
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//生成控制点的蒙皮数据。
			nProcessedControlPointCount = 0;
			ParseAllControlPointSkinWeight(pSceneRootNode, pControlPointGroup, &nProcessedControlPointCount);
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//生成骨骼数据。
			int nBoneTotalCount = 0;
			ParseBoneTotalCount(pSceneRootNode, &nBoneTotalCount);
			if (nBoneTotalCount > 0)
			{
				pBoneGroup->ReserveBoneCount(nBoneTotalCount);
				ParseBoneHierarchy(pSceneRootNode, pBoneGroup, -1);
				pBoneGroup->GenerateChildren();
				//计算骨骼的变换矩阵。
				ParseAllBoneMatrixFromBoneSpaceToWorldSpace(pSceneRootNode, pSceneRootNode, pBoneGroup);
				SoLogDebug("StFBXManager::LoadFBX : BoneTotalCount[%d]", pBoneGroup->GetSize());
				//之前，控制点的蒙皮数据中存储的是骨骼名字。
				//现在有了骨骼数据，可以将骨骼名字转换成相应的骨骼序号。
				pControlPointGroup->MakeBoneIndexByBoneName(pBoneGroup);
			}
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		}

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//生成动画帧数据。
		int nTheBoneCount = pBoneGroup->GetSize();
		if (nTheBoneCount > 0)
		{
			pModelAnimation->ReserveBoneCount(nTheBoneCount);
			ParseAllBoneAnimationData(pSDKScene, pBoneGroup, pModelAnimation);
		}
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		br = true;
	} while (0);

	if (pSDKImporter)
	{
		pSDKImporter->Destroy();
		pSDKImporter = 0;
	}
	if (pSDKScene)
	{
		pSDKScene->Destroy();
		pSDKScene = 0;
	}

	SoLogDebug("StFBXManager::LoadFBX : Finish; Success[%d]", br?1:0);
	return br;
}
//----------------------------------------------------------------
void StFBXManager::ParseMeshVertexTotalCountAndVertexType(FbxNode* pNode, SoBitFlag* pVertexType, int* pTotalCount, bool* pAllMeshOK)
{
	if (pNode == NULL)
	{
		return;
	}
	if (*pAllMeshOK == false)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			break;
		}

		//注意，先计算bFirstMesh值，再做其他检查。
		bool bFirstMesh = (*pTotalCount == 0);
		ParseSingleMeshVertexType(pMesh, pVertexType, pAllMeshOK, bFirstMesh);
		const int nPolygonCount = pMesh->GetPolygonCount();
		*pTotalCount += nPolygonCount * 3;

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseMeshVertexTotalCountAndVertexType(pNode->GetChild(i), pVertexType, pTotalCount, pAllMeshOK);
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseSingleMeshVertexType(FbxMesh* pFbxMesh, SoBitFlag* pVertexType, bool* pAllMeshOK, bool bFirstMesh)
{
	//肯定有顶点坐标数据
	if (bFirstMesh)
	{
		pVertexType->AddFlag(StFBXElement_Position);
	}
	if (pFbxMesh->GetElementMaterialCount() > 0)
	{
		const int nMappingMode = pFbxMesh->GetElementMaterial(0)->GetMappingMode();
		if (nMappingMode != FbxGeometryElement::eAllSame)
		{
			SoLogError("StFBXManager::ParseSingleMeshVertexType : Material MappingMode Error [%d]", nMappingMode);
			*pAllMeshOK = false;
			return;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在法线数据
	//获取且仅获取0号位置的法线数据。
	if (StFBX_ParseNormalData)
	{
		bool bNormalExist = false;
		if (pFbxMesh->GetElementNormalCount() > 0)
		{
			const int nMappingMode = pFbxMesh->GetElementNormal(0)->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				bNormalExist = true;
			}
			else
			{
				SoLogError("StFBXManager::ParseSingleMeshVertexType : Normal MappingMode Error [%d]", nMappingMode);
				*pAllMeshOK = false;
				return;
			}
		}
		if (bFirstMesh)
		{
			if (bNormalExist)
			{
				pVertexType->AddFlag(StFBXElement_Normal);
			}
		}
		else
		{
			if (pVertexType->IsFlagExist(StFBXElement_Normal) != bNormalExist)
			{
				//本Mesh与其他的Mesh有差异。
				SoLogError("StFBXManager::ParseSingleMeshVertexType : Normal Data different");
				*pAllMeshOK = false;
				return;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在切线数据
	//获取且仅获取0号位置的切线数据。
	if (StFBX_ParseTangentData)
	{
		bool bTangentExist = false;
		if (pFbxMesh->GetElementTangentCount() > 0)
		{
			const int nMappingMode = pFbxMesh->GetElementTangent(0)->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				bTangentExist = true;
			}
			else
			{
				SoLogError("StFBXManager::ParseSingleMeshVertexType : Tangent MappingMode Error [%d]", nMappingMode);
				*pAllMeshOK = false;
				return;
			}
		}
		if (bFirstMesh)
		{
			if (bTangentExist)
			{
				pVertexType->AddFlag(StFBXElement_Tangent);
			}
		}
		else
		{
			if (pVertexType->IsFlagExist(StFBXElement_Tangent) != bTangentExist)
			{
				//本Mesh与其他的Mesh有差异。
				SoLogError("StFBXManager::ParseSingleMeshVertexType : Tangent Data different");
				*pAllMeshOK = false;
				return;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在顶点颜色
	//获取且仅获取0号位置的顶点色数据。
	if (StFBX_ParseColorData)
	{
		bool bColorExist = false;
		if (pFbxMesh->GetElementVertexColorCount() > 0)
		{
			const int nMappingMode = pFbxMesh->GetElementVertexColor(0)->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				bColorExist = true;
			}
			else
			{
				SoLogError("StFBXManager::ParseSingleMeshVertexType : VertexColor MappingMode Error [%d]", nMappingMode);
				*pAllMeshOK = false;
				return;
			}
		}
		if (bFirstMesh)
		{
			if (bColorExist)
			{
				pVertexType->AddFlag(StFBXElement_Color);
			}
		}
		else
		{
			if (pVertexType->IsFlagExist(StFBXElement_Color) != bColorExist)
			{
				//本Mesh与其他的Mesh有差异。
				SoLogError("StFBXManager::ParseSingleMeshVertexType : VertexColor Data different");
				*pAllMeshOK = false;
				return;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在UV
	//获取且仅获取0号和1号位置的UV数据。
	if (StFBX_ParseUV1Data)
	{
		bool bUV1Exist = false;
		if (pFbxMesh->GetElementUVCount() > 0)
		{
			const int nMappingMode = pFbxMesh->GetElementUV(0)->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				bUV1Exist = true;
			}
			else
			{
				SoLogError("StFBXManager::ParseSingleMeshVertexType : UV1 MappingMode Error [%d]", nMappingMode);
				*pAllMeshOK = false;
				return;
			}
		}
		if (bFirstMesh)
		{
			if (bUV1Exist)
			{
				pVertexType->AddFlag(StFBXElement_UV1);
			}
		}
		else
		{
			if (pVertexType->IsFlagExist(StFBXElement_UV1) != bUV1Exist)
			{
				//本Mesh与其他的Mesh有差异。
				SoLogError("StFBXManager::ParseSingleMeshVertexType : UV1 Data different");
				*pAllMeshOK = false;
				return;
			}
		}
	}
	if (StFBX_ParseUV2Data)
	{
		bool bUV2Exist = false;
		if (pFbxMesh->GetElementUVCount() > 1)
		{
			const int nMappingMode = pFbxMesh->GetElementUV(1)->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				bUV2Exist = true;
			}
			else
			{
				SoLogError("StFBXManager::ParseSingleMeshVertexType : UV2 MappingMode Error [%d]", nMappingMode);
				*pAllMeshOK = false;
				return;
			}
		}
		if (bFirstMesh)
		{
			if (bUV2Exist)
			{
				pVertexType->AddFlag(StFBXElement_UV2);
			}
		}
		else
		{
			if (pVertexType->IsFlagExist(StFBXElement_UV2) != bUV2Exist)
			{
				//本Mesh与其他的Mesh有差异。
				SoLogError("StFBXManager::ParseSingleMeshVertexType : UV2 Data different");
				*pAllMeshOK = false;
				return;
			}
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseMeshData(FbxNode* pNode, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			break;
		}

		LoadSingleMesh(pMesh, pMeshData, pProcessedVertexCount, pProcessedControlPointCount);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseMeshData(pNode->GetChild(i), pMeshData, pProcessedVertexCount, pProcessedControlPointCount);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh(FbxMesh* pFbxMesh, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount)
{
	const int nFloatCountPerVertexData = pMeshData->nSizeofVertexData / sizeof(float);
	float* theVertexBuff = (float*)(pMeshData->pVertexBuff);
	unsigned int* theIndexBuff = (unsigned int*)(pMeshData->pIndexBuff);
	int* theIndex2ControlPointIndex = pMeshData->pVertexIndex2ControlPointIndex;	
	//
	const int nTriangleCount = pFbxMesh->GetPolygonCount();
	int nAccVertexCount = 0;
	FbxVector4 kVector4;
	FbxVector2 kVector2;
	FbxColor kColor;
	for (int i = 0; i < nTriangleCount; ++i)
	{
		//每个三角网格有3个顶点
		for (int j = 0; j < 3; ++j)
		{
			//nVertexIndex表示本顶点在所有顶点中的位置序号。
			const int nVertexIndex = *pProcessedVertexCount + nAccVertexCount;
			//nControlPointIndex表示本顶点受哪个控制点的影响。
			const int nControlPointIndex = pFbxMesh->GetPolygonVertex(i, j);

			//索引
			if (StFBX_TriangleIndex_123)
			{
				theIndexBuff[nVertexIndex] = nVertexIndex;
			}
			else
			{
				theIndexBuff[nVertexIndex] = (nVertexIndex - j) + (2 - j);
			}

			//顶点到控制点的映射
			theIndex2ControlPointIndex[nVertexIndex] = *pProcessedControlPointCount + nControlPointIndex;
			
			//顶点
			float* thisVertex = theVertexBuff + nFloatCountPerVertexData * nVertexIndex;
			int nAccFloat = 0;
			kVector4 = pFbxMesh->GetControlPointAt(nControlPointIndex);
			thisVertex[nAccFloat++] = (float)kVector4[0];
			thisVertex[nAccFloat++] = (float)kVector4[1];
			thisVertex[nAccFloat++] = (float)kVector4[2];

			//获取法线数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_Normal))
			{
				LoadSingleMesh_Normal(pFbxMesh, nAccVertexCount, &kVector4);
				thisVertex[nAccFloat++] = (float)kVector4[0];
				thisVertex[nAccFloat++] = (float)kVector4[1];
				thisVertex[nAccFloat++] = (float)kVector4[2];
			}

			//获取切线数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_Tangent))
			{
				LoadSingleMesh_Tangent(pFbxMesh, nAccVertexCount, &kVector4);
				thisVertex[nAccFloat++] = (float)kVector4[0];
				thisVertex[nAccFloat++] = (float)kVector4[1];
				thisVertex[nAccFloat++] = (float)kVector4[2];
			}

			//获取顶点色数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_Color))
			{
				LoadSingleMesh_Color(pFbxMesh, nAccVertexCount, &kColor);
				thisVertex[nAccFloat++] = (float)kColor.mRed;
				thisVertex[nAccFloat++] = (float)kColor.mGreen;
				thisVertex[nAccFloat++] = (float)kColor.mBlue;
				thisVertex[nAccFloat++] = (float)kColor.mAlpha;
			}

			//获取UV数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_UV1))
			{
				LoadSingleMesh_UV(pFbxMesh, 0, nAccVertexCount, &kVector2);
				thisVertex[nAccFloat++] = (float)kVector2[0];
				thisVertex[nAccFloat++] = StFBX_UVHeightInverse ? float(1.0 - kVector2[1]) : float(kVector2[1]);
			}
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_UV2))
			{
				LoadSingleMesh_UV(pFbxMesh, 1, nAccVertexCount, &kVector2);
				thisVertex[nAccFloat++] = (float)kVector2[0];
				thisVertex[nAccFloat++] = StFBX_UVHeightInverse ? float(1.0 - kVector2[1]) : float(kVector2[1]);
			}

			//
			++nAccVertexCount;
		}
	}

	//
	*pProcessedVertexCount += nAccVertexCount;
	*pProcessedControlPointCount += pFbxMesh->GetControlPointsCount();
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_Normal(FbxMesh* pFbxMesh, int nVertexIndex, FbxVector4* pNormal)
{
	FbxGeometryElementNormal* lNormalElement = pFbxMesh->GetElementNormal(0);
	const int nReferenceMode = lNormalElement->GetReferenceMode();
	const FbxLayerElementArrayTemplate<FbxVector4>& kNormalArray = lNormalElement->GetDirectArray();
	//
	int nIndex = nVertexIndex;
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		const FbxLayerElementArrayTemplate<int>& kIndexArray = lNormalElement->GetIndexArray();
		nIndex = kIndexArray.GetAt(nVertexIndex);
	}
	*pNormal = kNormalArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_Tangent(FbxMesh* pFbxMesh, int nVertexIndex, FbxVector4* pTangent)
{
	FbxGeometryElementTangent* leTangent = pFbxMesh->GetElementTangent(0);
	const int nReferenceMode = leTangent->GetReferenceMode();
	const FbxLayerElementArrayTemplate<FbxVector4>& kTangentArray = leTangent->GetDirectArray();
	//
	int nIndex = nVertexIndex;
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		const FbxLayerElementArrayTemplate<int>& kIndexArray = leTangent->GetIndexArray();
		nIndex = kIndexArray.GetAt(nVertexIndex);
	}
	*pTangent = kTangentArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_Color(FbxMesh* pFbxMesh, int nVertexIndex, FbxColor* pColor)
{
	FbxGeometryElementVertexColor* leVtxc = pFbxMesh->GetElementVertexColor(0);
	const int nReferenceMode = leVtxc->GetReferenceMode();
	FbxLayerElementArrayTemplate<FbxColor>& kColorArray = leVtxc->GetDirectArray();
	//
	int nIndex = nVertexIndex;
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& kIndexArray = leVtxc->GetIndexArray();
		nIndex = kIndexArray.GetAt(nVertexIndex);
	}
	*pColor = kColorArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_UV(FbxMesh* pFbxMesh, int nUVIndex, int nVertexIndex, FbxVector2* pUV)
{
	const FbxGeometryElementUV* leUV = pFbxMesh->GetElementUV(nUVIndex);
	const int nReferenceMode = leUV->GetReferenceMode();
	const FbxLayerElementArrayTemplate<FbxVector2>& kUVArray = leUV->GetDirectArray();
	//
	int nIndex = nVertexIndex;
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		const FbxLayerElementArrayTemplate<int>& kIndexArray = leUV->GetIndexArray();
		nIndex = kIndexArray.GetAt(nVertexIndex);
	}
	*pUV = kUVArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::ParseControlPointTotalCount(FbxNode* pNode, int* pTotalCount)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			break;
		}

		const int nCount = pMesh->GetControlPointsCount();
		*pTotalCount += nCount;

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseControlPointTotalCount(pNode->GetChild(i), pTotalCount);
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllControlPoint(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			break;
		}

		LoadControlPointFromMesh(pMesh, pControlPointGroup);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseAllControlPoint(pNode->GetChild(i), pControlPointGroup);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadControlPointFromMesh(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup)
{
	FbxVector4 kVector4;
	const int nCount = pMesh->GetControlPointsCount();
	for (int i = 0; i < nCount; ++i)
	{
		StFBXControlPoint* pPoint = pControlPointGroup->TakeNew();
		if (pPoint)
		{
			kVector4 = pMesh->GetControlPointAt(i);
			pPoint->kVertex.x = (float)kVector4[0];
			pPoint->kVertex.y = (float)kVector4[1];
			pPoint->kVertex.z = (float)kVector4[2];
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseBoneTotalCount(FbxNode* pNode, int* pTotalCount)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			break;
		}
	
		*pTotalCount += 1;

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseBoneTotalCount(pNode->GetChild(i), pTotalCount);
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseBoneHierarchy(FbxNode* pNode, StFBXBoneGroup* pGroup, int nParentIndex)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			break;
		}

		int nMyBoneIndex = -1;
		StFBXBone* pMyBone = pGroup->TakeNew(&nMyBoneIndex);
		if (pMyBone)
		{
			pMyBone->kBoneName = pNode->GetName();
			pMyBone->nParentIndex = nParentIndex;
			//FbxAMatrix kGlobalTran = pNode->EvaluateGlobalTransform();
			//kGlobalTran = kGlobalTran.Inverse();
			//ConvertFbxAMatrixToSoMathMatrix4(&kGlobalTran, &(pMyBone->kMatFromBoneSpaceToModelSpace));
			//以本节点做为父节点，接下来要遍历本节点的子节点。
			nParentIndex = nMyBoneIndex;
		}

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseBoneHierarchy(pNode->GetChild(i), pGroup, nParentIndex);
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllControlPointSkinWeight(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup, int* pProcessedControlPointCount)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			break;
		}

		LoadSingleControlPointSkinWeight(pMesh, pControlPointGroup, pProcessedControlPointCount);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseAllControlPointSkinWeight(pNode->GetChild(i), pControlPointGroup, pProcessedControlPointCount);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleControlPointSkinWeight(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup, int* pProcessedControlPointCount)
{
	const int nStartControlPointIndex = *pProcessedControlPointCount;
	*pProcessedControlPointCount += pMesh->GetControlPointsCount();
	//
	const int nDeformerCount = pMesh->GetDeformerCount();
	for (int deformerIndex = 0; deformerIndex < nDeformerCount; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* pSkin = reinterpret_cast<FbxSkin*>(pMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (pSkin == NULL)
		{
			continue;
		}

		const int nClusterCount = pSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < nClusterCount; ++clusterIndex)
		{
			FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
			const char* pBoneName = pCluster->GetLink()->GetName();
			// Associate each joint with the control points it affects
			const int nControlPointCount = pCluster->GetControlPointIndicesCount();
			const int* pCPIndexList = pCluster->GetControlPointIndices();
			const double* pWeightList = pCluster->GetControlPointWeights();
			for (int cpIndex = 0; cpIndex < nControlPointCount; ++cpIndex)
			{
				const int nControlPointIndex = pCPIndexList[cpIndex] + nStartControlPointIndex;
				StFBXControlPoint* pControlPoint = pControlPointGroup->GetAt(nControlPointIndex);
				if (pControlPoint)
				{
					float fWeight = (float)(pWeightList[cpIndex]);
					pControlPoint->AddBoneNameSkinWeight(pBoneName, fWeight);
				}
				else
				{
					SoLogError("StFBXManager::LoadSingleControlPointSkinWeight : Can not find ControlPoint by index [%d]", nControlPointIndex);
				}
			}
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, FbxNode* pRootNode, StFBXBoneGroup* pBoneGroup)
{
	if (pNode == NULL)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			break;
		}

		LoadSingleBoneMatrixFromBoneSpaceToWorldSpace(pNode, pMesh, pRootNode, pBoneGroup);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseAllBoneMatrixFromBoneSpaceToWorldSpace(pNode->GetChild(i), pRootNode, pBoneGroup);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, FbxMesh* pMesh, FbxNode* pRootNode, StFBXBoneGroup* pBoneGroup)
{
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix matGeometryTransform;
	matGeometryTransform.SetIdentity();
	GetGeometryTransformation(pNode, &matGeometryTransform);

	const int nDeformerCount = pMesh->GetDeformerCount();
	for (int deformerIndex = 0; deformerIndex < nDeformerCount; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* pSkin = reinterpret_cast<FbxSkin*>(pMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (pSkin == 0)
		{
			continue;
		}

		const int nClusterCount = pSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < nClusterCount; ++clusterIndex)
		{
			FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
			const char* pBoneName = pCluster->GetLink()->GetName();
			const int nBoneIndex = pBoneGroup->GetBoneIndexByBoneName(pBoneName);
			if (nBoneIndex == -1)
			{
				SoLogError("StFBXManager::LoadSingleBoneMatrixFromBoneSpaceToWorldSpace : Can not find bone by BoneName[%s]", pBoneName);
				continue;
			}

			StFBXBone* pMyBone = pBoneGroup->GetAt(nBoneIndex);
			FbxAMatrix transformMatrix;						
			FbxAMatrix transformLinkMatrix;					
			FbxAMatrix globalBindposeInverseMatrix;
			// The transformation of the mesh at binding time
			pCluster->GetTransformMatrix(transformMatrix);
			// The transformation of the cluster(joint) at binding time from joint space to world space
			pCluster->GetTransformLinkMatrix(transformLinkMatrix);
			//transformLinkMatrix = pBoneNode->EvaluateGlobalTransform();
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * matGeometryTransform;
			ConvertFbxAMatrixToSoMathMatrix4(&globalBindposeInverseMatrix, &(pMyBone->kMatFromBoneSpaceToModelSpace));
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllBoneAnimationData(FbxScene* pSDKScene, StFBXBoneGroup* pBoneGroup, StFBXModelAnimation* pModelAnim)
{
	//一般情况下，一个fbx文件中只包含一个动画，也即只包含一个FbxAnimStack对象。
	//把0号FbxAnimStack对象设置为当前要播放（提取）的动画。
	FbxAnimStack* pAnimStack = pSDKScene->GetSrcObject<FbxAnimStack>(0);
	if (pAnimStack == NULL)
	{
		SoLogDebug("StFBXManager::ParseAllBoneAnimationData : no FbxAnimStack found");
		return;
	}
	pSDKScene->SetCurrentAnimationStack(pAnimStack);


	const FbxTime::EMode theTimeMode = FbxTime::eFrames24;
	FbxTimeSpan kTimeSpan = pAnimStack->GetLocalTimeSpan();
	FbxTime kAnimDuration = kTimeSpan.GetDuration();
	const float fAnimLength = (float)kAnimDuration.GetSecondDouble();
	const int nFrameCount = (int)kAnimDuration.GetFrameCount(theTimeMode);
	const int nBoneCount = pBoneGroup->GetSize();
	pModelAnim->nFrameCount = nFrameCount;
	pModelAnim->fAnimLength = fAnimLength;

	for (int boneIndex = 0; boneIndex < nBoneCount; ++boneIndex)
	{
		const char* szBoneName = pBoneGroup->GetAt(boneIndex)->kBoneName.GetValue();
		FbxNode* pBoneNode = NULL;
		int nAccBoneCount = 0;
		FindBoneByIndexAndName(pSDKScene->GetRootNode(), boneIndex, szBoneName, &pBoneNode, &nAccBoneCount);
		if (pBoneNode == NULL)
		{
			SoLogError("StFBXManager::ParseAllBoneAnimationData : Can not find Bone [%s]", szBoneName);
			continue;
		}

		StFBXBoneAnimation* pBoneAnim = pModelAnim->TakeNew();
		pBoneAnim->ReserveKeyFrameCount(nFrameCount);
		pBoneAnim->nBoneIndex = boneIndex;
		FbxTime kTime;
		for (int frameIndex = 0; frameIndex < nFrameCount; ++frameIndex)
		{
			StFBXKeyFrame* pKeyFrame = pBoneAnim->TakeNew();
			kTime.SetFrame(frameIndex, theTimeMode);
			pKeyFrame->fKeyTime = (float)kTime.GetSecondDouble();
			const FbxAMatrix& kGlobalTran = pBoneNode->EvaluateGlobalTransform(kTime);
			ConvertFbxAMatrixToSoMathMatrix4(&kGlobalTran, &(pKeyFrame->matKeyTransform));
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::GetGeometryTransformation(FbxNode* pNode, FbxAMatrix* pMat)
{
	if (pMat)
	{
		if (pNode)
		{
			const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
			const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
			const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
			*pMat = FbxAMatrix(lT, lR, lS);
		}
		else
		{
			pMat->SetIdentity();
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ConvertFbxAMatrixToSoMathMatrix4(const FbxAMatrix* pFbxAMatrix, SoMathMatrix4* pSoMatrix)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			pSoMatrix->element[row][column] = (float)pFbxAMatrix->Get(row, column);
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::FindBoneByIndexAndName(FbxNode* pNode, int nBoneIndex, const char* szBoneName, FbxNode** ppResultNode, int* pAccBoneCount)
{
	if (pNode == NULL)
	{
		return;
	}
	if (*ppResultNode != NULL)
	{
		//找到了。
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == NULL)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			break;
		}

		if (*pAccBoneCount == nBoneIndex)
		{
			if (SoStrCmpNoCase(pNode->GetName(), szBoneName) == 0)
			{
				*ppResultNode = pNode;
				break;
			}
		}

		*pAccBoneCount += 1;

	} while (0);

	if (*ppResultNode != NULL)
	{
		//找到了。
		return;
	}

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		FindBoneByIndexAndName(pNode->GetChild(i), nBoneIndex, szBoneName, ppResultNode, pAccBoneCount);
	}
}
//----------------------------------------------------------------
