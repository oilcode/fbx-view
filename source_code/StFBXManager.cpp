//----------------------------------------------------------------
#include "StFBXManager.h"
#include "StFBXModel.h"
#include "SoSimpleLog.h"
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

		//// Convert Unit System to what is used in this example, if needed
		//FbxSystemUnit SceneSystemUnit = pSDKScene->GetGlobalSettings().GetSystemUnit();
		//if (SceneSystemUnit.GetScaleFactor() != 1.0)
		//{
		//	//The unit in this example is centimeter.
		//	FbxSystemUnit::cm.ConvertScene(pSDKScene);
		//}

		//FBX文件支持Nurbes，Polygon，Triangle等网格类型，
		//这个函数用来把非Triangle网格转换成Triangle网格类型。
		//如果你确信FBX文件中只含有Triangle网格，那么可以跳过本函数。
		FbxGeometryConverter lGeomConverter(m_pSDKManager);
		lGeomConverter.Triangulate(pSDKScene, true);

		SoLogDebug("StFBXManager::LoadFBX : Load file success");

		//开始解析
		StFBXMeshData* pMeshData = pFBXModel->GetMeshData_Modify();
		StFBXControlPointGroup* pControlPointGroup = pFBXModel->GetControlPointGroup_Modify();
		StFBXBoneGroup* pBoneGroup = pFBXModel->GetBoneGroup_Modify();
		StFBXModelAnimation* pModelAnimation = pFBXModel->GetAnimation_Modify();

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//生成Mesh顶点数据。
		int nMeshVertexTotalCount = 0;
		ParveMeshVertexTotalCountAndVertexType(pSDKScene->GetRootNode(), &nMeshVertexTotalCount, pMeshData);
		pMeshData->ReserveVertexCount(nMeshVertexTotalCount);
		int nProcessedVertexCount = 0;
		int nProcessedControlPointCount = 0;
		ParseMeshData(pSDKScene->GetRootNode(), pMeshData, &nProcessedVertexCount, &nProcessedControlPointCount);
		SoLogDebug("StFBXManager::LoadFBX : VertexCount[%d] VertexType[%u] VertexStructSize[%d]", pMeshData->nVertexCount, pMeshData->kVertexType.GetValue(), pMeshData->nSizeofVertexData);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//生成控制点数据。
		int nControlPointTotalCount = 0;
		ParseControlPointTotalCount(pSDKScene->GetRootNode(), &nControlPointTotalCount);
		pControlPointGroup->ReserveControlPointCount(nControlPointTotalCount);
		ParseAllControlPoint(pSDKScene->GetRootNode(), pControlPointGroup);
		SoLogDebug("StFBXManager::LoadFBX : ControlPointTotalCount[%d]", pControlPointGroup->GetSize());
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//生成骨骼数据。
		int nBoneTotalCount = 0;
		ParseBoneTotalCount(pSDKScene->GetRootNode(), &nBoneTotalCount);
		if (nBoneTotalCount > 0)
		{
			pBoneGroup->ReserveBoneCount(nBoneTotalCount);
			ParseBoneHierarchy(pSDKScene->GetRootNode(), pBoneGroup, -1);
			pBoneGroup->GenerateChildren();
			ParseAllBoneMatrixFromBoneSpaceToWorldSpace(pSDKScene->GetRootNode(), pBoneGroup);
			SoLogDebug("StFBXManager::LoadFBX : BoneTotalCount[%d]", pBoneGroup->GetSize());
		}
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//生成蒙皮数据。
		if (nBoneTotalCount > 0)
		{
			nProcessedControlPointCount = 0;
			ParseAllBoneSkinWeight(pSDKScene->GetRootNode(), pControlPointGroup, pBoneGroup, &nProcessedControlPointCount);
			//pControlPointGroup->CheckBoneSkinWeight();
		}
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//生成动画帧数据。
		if (nBoneTotalCount > 0)
		{
			FbxTime kBeginTime(0);
			FbxTime kEndTime(0);
			GetAnimationTimeLength(pSDKScene, &kBeginTime, &kEndTime);
			pModelAnimation->ReserveBoneCount(nBoneTotalCount);
			ParseAllBoneAnimationData(pSDKScene->GetRootNode(), &kBeginTime, &kEndTime, pBoneGroup, pModelAnimation);
			//
			StFBXBoneAnimation* _pAnim = pModelAnimation->GetAt(0);
			if (_pAnim)
			{
				SoLogDebug("StFBXManager::LoadFBX : TimeLength[%.2f] KeyFrameCount[%d]", pModelAnimation->fAnimLength, _pAnim->GetSize());
			}
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
void StFBXManager::ParveMeshVertexTotalCountAndVertexType(FbxNode* pNode, int* pTotalCount, StFBXMeshData* pMeshData)
{
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
		{
			break;
		}

		const int nPolygonCount = pMesh->GetPolygonCount();
		*pTotalCount += nPolygonCount * 3;

		//解析顶点结构体中含有哪些成员。
		//只需解析一次，所以值为0时才解析。
		if (pMeshData->kVertexType.IsZero())
		{
			ParseVertexType(pMesh, pMeshData);
		}

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParveMeshVertexTotalCountAndVertexType(pNode->GetChild(i), pTotalCount, pMeshData);
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseMeshData(FbxNode* pNode, StFBXMeshData* pMeshData, int* pProcessedVertexCount, int* pProcessedControlPointCount)
{
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
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
	unsigned int* theIndexBuff = (unsigned int*)(pMeshData->pIndexBuff);
	int* theIndex2ControlPointIndex = pMeshData->pVertexIndex2ControlPointIndex;
	float* theVertexBuff = (float*)(pMeshData->pVertexBuff);
	const int nFloatCountPerVertexData = pMeshData->nSizeofVertexData / sizeof(float);
	
	//
	const int nTriangleCount = pFbxMesh->GetPolygonCount();
	int nAccVertexCount = 0;
	FbxVector4 kVector4;
	FbxColor kColor;
	FbxVector2 kVector2;
	for (int i = 0; i < nTriangleCount; ++i)
	{
		//每个三角网格有3个顶点
		for (int j = 0; j < 3; ++j)
		{
			//nVertexIndex表示在所有顶点中的位置序号。
			const int nVertexIndex = *pProcessedVertexCount + nAccVertexCount;
			//nControlPointIndex表示在本FbxMesh中的位置序号。
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
				LoadSingleMesh_Normal(pFbxMesh, nControlPointIndex, nAccVertexCount, &kVector4);
				thisVertex[nAccFloat++] = (float)kVector4[0];
				thisVertex[nAccFloat++] = (float)kVector4[1];
				thisVertex[nAccFloat++] = (float)kVector4[2];
			}

			//获取切线数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_Tangent))
			{
				LoadSingleMesh_Tangent(pFbxMesh, nControlPointIndex, nAccVertexCount, &kVector4);
				thisVertex[nAccFloat++] = (float)kVector4[0];
				thisVertex[nAccFloat++] = (float)kVector4[1];
				thisVertex[nAccFloat++] = (float)kVector4[2];
			}

			//获取顶点色数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_Color))
			{
				LoadSingleMesh_Color(pFbxMesh, nControlPointIndex, nAccVertexCount, &kColor);
				thisVertex[nAccFloat++] = (float)kColor.mRed;
				thisVertex[nAccFloat++] = (float)kColor.mGreen;
				thisVertex[nAccFloat++] = (float)kColor.mBlue;
				thisVertex[nAccFloat++] = (float)kColor.mAlpha;
			}

			//获取UV数据
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_UV1))
			{
				LoadSingleMesh_UV(pFbxMesh, 0, nControlPointIndex, nAccVertexCount, &kVector2);
				thisVertex[nAccFloat++] = (float)kVector2[0];
				thisVertex[nAccFloat++] = (float)kVector2[1];
			}
			if (pMeshData->kVertexType.IsFlagExist(StFBXElement_UV2))
			{
				LoadSingleMesh_UV(pFbxMesh, 1, nControlPointIndex, nAccVertexCount, &kVector2);
				thisVertex[nAccFloat++] = (float)kVector2[0];
				thisVertex[nAccFloat++] = (float)kVector2[1];
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
void StFBXManager::LoadSingleMesh_Normal(FbxMesh* pFbxMesh, int nControlPointIndex, int nAccVertexCount, FbxVector4* pNormal)
{
	FbxGeometryElementNormal* lNormalElement = pFbxMesh->GetElementNormal(0);
	if (lNormalElement == 0)
	{
		return;
	}

	const int nMappingMode = lNormalElement->GetMappingMode();
	const int nReferenceMode = lNormalElement->GetReferenceMode();
	const FbxLayerElementArrayTemplate<int>& kIndexArray = lNormalElement->GetIndexArray();
	const FbxLayerElementArrayTemplate<FbxVector4>& kNormalArray = lNormalElement->GetDirectArray();

	int nIndex = nAccVertexCount;
	if (nMappingMode == FbxGeometryElement::eByControlPoint)
	{
		nIndex = nControlPointIndex;
	}
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		nIndex = kIndexArray.GetAt(nIndex);
	}
	*pNormal = kNormalArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_Tangent(FbxMesh* pFbxMesh, int nControlPointIndex, int nAccVertexCount, FbxVector4* pTangent)
{
	FbxGeometryElementTangent* leTangent = pFbxMesh->GetElementTangent(0);
	if (leTangent == 0)
	{
		return;
	}

	const int nMappingMode = leTangent->GetMappingMode();
	const int nReferenceMode = leTangent->GetReferenceMode();
	const FbxLayerElementArrayTemplate<int>& kIndexArray = leTangent->GetIndexArray();
	const FbxLayerElementArrayTemplate<FbxVector4>& kTangentArray = leTangent->GetDirectArray();

	int nIndex = nAccVertexCount;
	if (nMappingMode == FbxGeometryElement::eByControlPoint)
	{
		nIndex = nControlPointIndex;
	}
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		nIndex = kIndexArray.GetAt(nIndex);
	}
	*pTangent = kTangentArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_Color(FbxMesh* pFbxMesh, int nControlPointIndex, int nAccVertexCount, FbxColor* pColor)
{
	FbxGeometryElementVertexColor* leVtxc = pFbxMesh->GetElementVertexColor(0);
	if (leVtxc == 0)
	{
		return;
	}

	const int nMappingMode = leVtxc->GetMappingMode();
	const int nReferenceMode = leVtxc->GetReferenceMode();
	FbxLayerElementArrayTemplate<int>& kIndexArray = leVtxc->GetIndexArray();
	FbxLayerElementArrayTemplate<FbxColor>& kColorArray = leVtxc->GetDirectArray();
	
	int nIndex = nAccVertexCount;
	if (nMappingMode == FbxGeometryElement::eByControlPoint)
	{
		nIndex = nControlPointIndex;
	}
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		nIndex = kIndexArray.GetAt(nIndex);
	}
	*pColor = kColorArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleMesh_UV(FbxMesh* pFbxMesh, int nUVIndex, int nControlPointIndex, int nAccVertexCount, FbxVector2* pUV)
{
	const FbxGeometryElementUV* leUV = pFbxMesh->GetElementUV(nUVIndex);
	if (leUV == 0)
	{
		return;
	}

	const int nMappingMode = leUV->GetMappingMode();
	const int nReferenceMode = leUV->GetReferenceMode();
	const FbxLayerElementArrayTemplate<int>& kIndexArray = leUV->GetIndexArray();
	const FbxLayerElementArrayTemplate<FbxVector2>& kUVArray = leUV->GetDirectArray();

	int nIndex = nAccVertexCount;
	if (nMappingMode == FbxGeometryElement::eByControlPoint)
	{
		nIndex = nControlPointIndex;
	}
	if (nReferenceMode == FbxGeometryElement::eIndexToDirect)
	{
		nIndex = kIndexArray.GetAt(nIndex);
	}
	//
	*pUV = kUVArray.GetAt(nIndex);
}
//----------------------------------------------------------------
void StFBXManager::ParseVertexType(FbxMesh* pFbxMesh, StFBXMeshData* pMeshData)
{
	//肯定有顶点坐标数据
	pMeshData->kVertexType.AddFlag(StFBXElement_Position);
	pMeshData->nSizeofVertexData += StFBX_Sizeof_Vector3;
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在法线数据
	//获取且仅获取0号位置的法线数据。
	if (StFBX_ParseNormalData)
	{
		bool bNormalExist = false;
		FbxGeometryElementNormal* lNormalElement = pFbxMesh->GetElementNormal(0);
		if (lNormalElement)
		{
			const int nMappingMode = lNormalElement->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByControlPoint || nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				const int nReferenceMode = lNormalElement->GetReferenceMode();
				if (nReferenceMode == FbxGeometryElement::eDirect || nReferenceMode == FbxGeometryElement::eIndexToDirect)
				{
					bNormalExist = true;
				}
			}
		}
		if (bNormalExist)
		{
			pMeshData->kVertexType.AddFlag(StFBXElement_Normal);
			pMeshData->nSizeofVertexData += StFBX_Sizeof_Vector3;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在切线数据
	//获取且仅获取0号位置的切线数据。
	if (StFBX_ParseTangentData)
	{
		bool bTangentExist = false;
		FbxGeometryElementTangent* leTangent = pFbxMesh->GetElementTangent(0);
		if (leTangent)
		{
			const int nMappingMode = leTangent->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByControlPoint || nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				const int nReferenceMode = leTangent->GetReferenceMode();
				if (nReferenceMode == FbxGeometryElement::eDirect || nReferenceMode == FbxGeometryElement::eIndexToDirect)
				{
					bTangentExist = true;
				}
			}
		}
		if (bTangentExist)
		{
			pMeshData->kVertexType.AddFlag(StFBXElement_Tangent);
			pMeshData->nSizeofVertexData += StFBX_Sizeof_Vector3;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在顶点颜色
	//获取且仅获取0号位置的顶点色数据。
	if (StFBX_ParseColorData)
	{
		bool bColorExist = false;
		FbxGeometryElementVertexColor* leVtxc = pFbxMesh->GetElementVertexColor(0);
		if (leVtxc)
		{
			const int nMappingMode = leVtxc->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByControlPoint || nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				const int nReferenceMode = leVtxc->GetReferenceMode();
				if (nReferenceMode == FbxGeometryElement::eDirect || nReferenceMode == FbxGeometryElement::eIndexToDirect)
				{
					bColorExist = true;
				}
			}
		}
		if (bColorExist)
		{
			pMeshData->kVertexType.AddFlag(StFBXElement_Color);
			pMeshData->nSizeofVertexData += StFBX_Sizeof_Color;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//判断是否存在UV
	//获取且仅获取0号和1号位置的UV数据。
	if (StFBX_ParseUV1Data)
	{
		bool bUV1Exist = false;
		const FbxGeometryElementUV* lUVElement = pFbxMesh->GetElementUV(0);
		if (lUVElement)
		{
			const int nMappingMode = lUVElement->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByControlPoint || nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				const int nReferenceMode = lUVElement->GetReferenceMode();
				if (nReferenceMode == FbxGeometryElement::eDirect || nReferenceMode == FbxGeometryElement::eIndexToDirect)
				{
					bUV1Exist = true;
				}
			}
		}
		if (bUV1Exist)
		{
			pMeshData->kVertexType.AddFlag(StFBXElement_UV1);
			pMeshData->nSizeofVertexData += StFBX_Sizeof_UV;
		}
	}
	if (StFBX_ParseUV2Data)
	{
		bool bUV2Exist = false;
		const FbxGeometryElementUV* lUVElement = pFbxMesh->GetElementUV(1);
		if (lUVElement)
		{
			const int nMappingMode = lUVElement->GetMappingMode();
			if (nMappingMode == FbxGeometryElement::eByControlPoint || nMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				const int nReferenceMode = lUVElement->GetReferenceMode();
				if (nReferenceMode == FbxGeometryElement::eDirect || nReferenceMode == FbxGeometryElement::eIndexToDirect)
				{
					bUV2Exist = true;
				}
			}
		}
		if (bUV2Exist)
		{
			pMeshData->kVertexType.AddFlag(StFBXElement_UV2);
			pMeshData->nSizeofVertexData += StFBX_Sizeof_UV;
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseControlPointTotalCount(FbxNode* pNode, int* pTotalCount)
{
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
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
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
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
			pPoint->ResetInit();
			//
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
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
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
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			break;
		}

		int nBoneIndex = -1;
		StFBXBone* pBone = pGroup->TakeNew(&nBoneIndex);
		if (pBone)
		{
			pBone->ResetInit();
			//
			pBone->kBoneName.SetValue(pNode->GetName());
			pBone->nParentIndex = nParentIndex;
			//以本节点做为父节点，接下来要遍历本节点的子节点。
			nParentIndex = nBoneIndex;
		}

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseBoneHierarchy(pNode->GetChild(i), pGroup, nParentIndex);
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllBoneSkinWeight(FbxNode* pNode, StFBXControlPointGroup* pControlPointGroup, StFBXBoneGroup* pBoneGroup, int* pProcessedControlPointCount)
{
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
		{
			break;
		}

		LoadSingleBoneSkinWeight(pMesh, pControlPointGroup, pBoneGroup, pProcessedControlPointCount);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseAllBoneSkinWeight(pNode->GetChild(i), pControlPointGroup, pBoneGroup, pProcessedControlPointCount);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleBoneSkinWeight(FbxMesh* pMesh, StFBXControlPointGroup* pControlPointGroup, StFBXBoneGroup* pBoneGroup, int* pProcessedControlPointCount)
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
				SoLogError("StFBXManager::LoadSingleBoneSkinWeight : Can not find bone by BoneName[%s]", pBoneName);
				continue;
			}

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
					pControlPoint->AddBoneIndexSkinWeight(nBoneIndex, fWeight);
				}
			}
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, StFBXBoneGroup* pBoneGroup)
{
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
		{
			break;
		}

		LoadSingleBoneMatrixFromBoneSpaceToWorldSpace(pNode, pMesh, pBoneGroup);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseAllBoneMatrixFromBoneSpaceToWorldSpace(pNode->GetChild(i), pBoneGroup);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleBoneMatrixFromBoneSpaceToWorldSpace(FbxNode* pNode, FbxMesh* pMesh, StFBXBoneGroup* pBoneGroup)
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
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * matGeometryTransform;
			ConvertFbxAMatrixToSoMathMatrix4(&globalBindposeInverseMatrix, &(pMyBone->kMatFromBoneSpaceToModelSpace));
		}
	}
}
//----------------------------------------------------------------
void StFBXManager::ParseAllBoneAnimationData(FbxNode* pNode, FbxTime* pBeginTime, FbxTime* pEndTime, StFBXBoneGroup* pBoneGroup, StFBXModelAnimation* pModelAnim)
{
	if (pNode == 0)
	{
		return;
	}

	do 
	{
		const FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();
		if (pNodeAttr == 0)
		{
			break;
		}
		if (pNodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			break;
		}
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == 0)
		{
			break;
		}

		LoadSingleBoneAnimationData(pNode, pMesh, pBeginTime, pEndTime, pBoneGroup, pModelAnim);

	} while (0);

	const int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		ParseAllBoneAnimationData(pNode->GetChild(i), pBeginTime, pEndTime, pBoneGroup, pModelAnim);
	}
}
//----------------------------------------------------------------
void StFBXManager::LoadSingleBoneAnimationData(FbxNode* pNode, FbxMesh* pMesh, FbxTime* pBeginTime, FbxTime* pEndTime, StFBXBoneGroup* pBoneGroup, StFBXModelAnimation* pModelAnim)
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
				SoLogError("StFBXManager::LoadSingleBoneAnimationData : Can not find bone by BoneName[%s]", pBoneName);
				continue;
			}

			StFBXBoneAnimation* pBoneAnim = pModelAnim->TakeNew();
			if (pBoneAnim == NULL)
			{
				continue;
			}

			pBoneAnim->ResetInit();
			pBoneAnim->nBoneIndex = nBoneIndex;

			FbxLongLong kBeginTimeFrame = pBeginTime->GetFrameCount(FbxTime::eFrames24);
			FbxLongLong kEndTimeFrame = pEndTime->GetFrameCount(FbxTime::eFrames24);
			const int nKeyFrameCount = (int)(kEndTimeFrame - kBeginTimeFrame) + 1;
			pBoneAnim->ReserveKeyFrameCount(nKeyFrameCount);

			FbxTime kKeyFrameTime;
			kKeyFrameTime.SetFrame(nKeyFrameCount, FbxTime::eFrames24);
			pModelAnim->fAnimLength = (float)kKeyFrameTime.GetMilliSeconds() * 0.001f;

			FbxNode* pLink = pCluster->GetLink();
			for (FbxLongLong frameIndex = 0; frameIndex < nKeyFrameCount; ++frameIndex)
			{
				StFBXKeyFrame* pKeyFrame = pBoneAnim->TakeNew();
				if (pKeyFrame)
				{
					kKeyFrameTime.SetFrame(frameIndex, FbxTime::eFrames24);
					pKeyFrame->fKeyTime = (float)kKeyFrameTime.GetMilliSeconds() * 0.001f;
					//
					kKeyFrameTime.SetFrame(frameIndex+kBeginTimeFrame, FbxTime::eFrames24);
					FbxAMatrix currentTransformOffset = pNode->EvaluateGlobalTransform(kKeyFrameTime) * matGeometryTransform;
					FbxAMatrix finalAnimMatrix = currentTransformOffset.Inverse() * pLink->EvaluateGlobalTransform(kKeyFrameTime);
					ConvertFbxAMatrixToSoMathMatrix4(&finalAnimMatrix, &(pKeyFrame->matKeyTransform));
				}
			}
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
void StFBXManager::ConvertFbxAMatrixToSoMathMatrix4(FbxAMatrix* pFbxAMatrix, SoMathMatrix4* pSoMatrix)
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
void StFBXManager::GetAnimationTimeLength(FbxScene* pSDKScene, FbxTime* pBeginTime, FbxTime* pEndTime)
{
	if (pSDKScene)
	{
		FbxAnimStack* pAnimStack = pSDKScene->GetSrcObject<FbxAnimStack>(0);
		if (pAnimStack)
		{
			FbxString strStackName = pAnimStack->GetName();
			FbxTakeInfo* pTakeInfo = pSDKScene->GetTakeInfo(strStackName);
			if (pTakeInfo)
			{
				*pBeginTime = pTakeInfo->mLocalTimeSpan.GetStart();
				*pEndTime = pTakeInfo->mLocalTimeSpan.GetStop();
			}
		}
	}
}
//----------------------------------------------------------------
