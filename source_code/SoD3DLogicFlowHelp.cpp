//----------------------------------------------------------------
#include "SoD3DLogicFlowHelp.h"
#include "SoD3DSystem.h"
#include "SoD3DTextureManager.h"
#include "SoD3DShaderManager.h"
#include "SoD3DModelManager.h"
#include "SoD3DCamera.h"
#include "SoD3DCameraUI.h"
#include "SoD3DTerrain.h"
#include "StFBXManager.h"
#include "StFBXModel.h"
#include "StKKFileKkbWrite.h"
#include "StKKFileKkbRead.h"
#include "StKKFileKkfWrite.h"
#include "StKKFileKkfRead.h"
#include "StKKModel.h"
#include "StKKFileKkmRead.h"
#include "NwUIMain.h"
//----------------------------------------------------------------
SoD3DModelFbx* g_pD3DModelFbx = 0;
StFBXModel* g_pFbxModel = 0;
SoD3DModelKK* g_pD3DModelKK = 0;
StKKModel* g_pKKModel = 0;
char g_buff[512];
//----------------------------------------------------------------
bool SoD3DLogicFlowHelp_Create()
{
	///////////////////////////////////////
	if (SoD3DSystem::CreateD3DSystem() == false)
	{
		return false;
	}
	///////////////////////////////////////
	if (SoD3DTextureManager::CreateD3DTextureManager() == false)
	{
		return false;
	}
	///////////////////////////////////////
	if (SoD3DShaderManager::CreateD3DShaderManager() == false)
	{
		return false;
	}
	if (SoD3DModelManager::CreateD3DModelManager() == false)
	{
		return false;
	}
	///////////////////////////////////////
	SoD3DCamera::CreateD3DCamera();
	SoD3DCameraUI::CreateD3DCameraUI();
	SoD3DTerrain::CreateD3DTerrain();
	///////////////////////////////////////
	if (StFBXManager::CreateFBXManager() == false)
	{
		SoLogError("SoD3DLogicFlowHelp_Create : StFBXManager::CreateFBXManager() fail");
		return false;
	}
	///////////////////////////////////////

	return true;
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_Release()
{
	if (g_pFbxModel)
	{
		delete g_pFbxModel;
		g_pFbxModel = 0;
	}
	if (g_pD3DModelFbx)
	{
		delete g_pD3DModelFbx;
		g_pD3DModelFbx = 0;
	}
	if (g_pKKModel)
	{
		delete g_pKKModel;
		g_pKKModel = 0;
	}
	if (g_pD3DModelKK)
	{
		delete g_pD3DModelKK;
		g_pD3DModelKK = 0;
	}
	///////////////////////////////////////
	StFBXManager::ReleaseFBXManager();
	///////////////////////////////////////
	SoD3DTerrain::ReleaseD3DTerrain();
	SoD3DCameraUI::ReleaseD3DCameraUI();
	SoD3DCamera::ReleaseD3DCamera();
	///////////////////////////////////////
	SoD3DModelManager::ReleaseD3DModelManager();
	SoD3DShaderManager::ReleaseD3DShaderManager();
	SoD3DTextureManager::ReleaseD3DTextureManager();
	SoD3DSystem::ReleaseD3DSystem();
	///////////////////////////////////////
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_Update(float fDeltaTime)
{
	if (SoD3DCamera::Get())
	{
		SoD3DCamera::Get()->UpdateFreeMove(fDeltaTime);
		SoD3DCamera::Get()->UpdateViewMatrix();
	}
	if (g_pD3DModelFbx)
	{
		g_pD3DModelFbx->UpdateModel(fDeltaTime, g_pFbxModel);
	}
	if (g_pD3DModelKK)
	{
		g_pD3DModelKK->UpdateModel(fDeltaTime);
	}
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_PreRender()
{

}
//----------------------------------------------------------------
bool SoD3DLogicFlowHelp_BeginRender()
{
	if (SoD3DSystem::Get() == NULL)
	{
		return false;
	}

	SoD3DSystem::Get()->BeginRender();

	if (SoD3DTerrain::Get())
	{
		SoD3DTerrain::Get()->RenderD3DTerrain();
	}

	if (g_pD3DModelFbx)
	{
		g_pD3DModelFbx->RenderModel();
	}

	if (g_pD3DModelKK)
	{
		g_pD3DModelKK->RenderModel();
	}

	return true;
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_EndRender()
{
	SoD3DSystem::Get()->EndRender();
}
//----------------------------------------------------------------
void LoadFbxAnim()
{
	//加载的fbx文件只包含动画信息，例如"S_M_065@Idle.FBX"。
	if (g_pFbxModel == NULL || g_pD3DModelFbx == NULL)
	{
		return;
	}

	if (StFBXManager::Get()->LoadFBX(g_buff, g_pFbxModel) == false)
	{
		const char* szText = SoStrFmt("加载fbx文件失败 %s", g_buff);
		NwUIMain::Get()->SetLog(szText);
		return;
	}

	SoD3DLogicFlowHelp_ConvertFbx2KK();

	//更新UI
	NwUIMain::Get()->SetLog(g_buff);
	NwUIMain::Get()->SetVertexCount(g_pFbxModel->GetMeshData()->nVertexCount);
	NwUIMain::Get()->SetControlPointCount(g_pFbxModel->GetControlPointGroup()->GetSize());
	NwUIMain::Get()->SetAnimLength(g_pFbxModel->GetAnimTimeLength());
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_OnDropFileFbx(const char* szFileName)
{
	SoStrCpy(g_buff, sizeof(g_buff), szFileName);
	const int nPos = SoStrRChr(g_buff, '.');
	if (nPos == -1)
	{
		return;
	}

	//加载的fbx文件只包含动画信息，例如"S_M_065@Idle.FBX"。
	if (SoStrChr(g_buff, '@') != -1)
	{
		LoadFbxAnim();
		return;
	}


	if (g_pFbxModel)
	{
		delete g_pFbxModel;
		g_pFbxModel = 0;
	}
	if (g_pD3DModelFbx)
	{
		delete g_pD3DModelFbx;
		g_pD3DModelFbx = 0;
	}
	if (g_pKKModel)
	{
		delete g_pKKModel;
		g_pKKModel = 0;
	}
	if (g_pD3DModelKK)
	{
		delete g_pD3DModelKK;
		g_pD3DModelKK = 0;
	}

	g_pFbxModel = new StFBXModel;
	if (StFBXManager::Get()->LoadFBX(g_buff, g_pFbxModel) == false)
	{
		const char* szText = SoStrFmt("加载fbx文件失败 %s", g_buff);
		NwUIMain::Get()->SetLog(szText);
		return;
	}

	SoD3DLogicFlowHelp_ConvertFbx2KK();

	//
	SoD3DModelInitParam_Fbx FbxParam;
	FbxParam.pMeshData = g_pFbxModel->GetMeshData();
	g_pD3DModelFbx = new SoD3DModelFbx;
	g_pD3DModelFbx->InitModel(&FbxParam);


	//以地表为参照物，把模型缩放到合适的大小。
	const float fTerrainWidth = 10.0f;
	//const float fTerrainHeight = 10.0f;
	const float fAdjustScale = 0.5f;
	SoMathFloat3 kMinPos;
	SoMathFloat3 kMaxPos;
	g_pFbxModel->CalculateMeshBoundingBox(&kMinPos, &kMaxPos);
	float fScale = 0.01f;
	if (SoMath_IsFloatZero(kMaxPos.x - kMinPos.x) == false)
	{
		fScale = fTerrainWidth / (kMaxPos.x - kMinPos.x);
		fScale *= fAdjustScale;
	}

	//模型的最低点位于怎样的高度
	const float fModelRootPosZ = 0.0f;
	float fPosZ = (fModelRootPosZ - kMinPos.z) * fScale;

	float fPosX = 5.0f;
	float fPosY = 5.0f;
	XMMATRIX matRotate = XMMatrixRotationZ(0.0f);
	XMMATRIX matScale = XMMatrixScaling(fScale, fScale, fScale);
	XMMATRIX matTranslate = XMMatrixTranslation(fPosX, fPosY, fPosZ);
	XMMATRIX matWorld = XMMatrixMultiply(matRotate, matScale);
	matWorld = XMMatrixMultiply(matWorld, matTranslate);
	XMFLOAT4X4 kMatWorld;
	XMStoreFloat4x4(&kMatWorld, matWorld);

	g_pD3DModelFbx->SetWorldMatrix(&kMatWorld);

	//更新UI
	NwUIMain::Get()->SetLog(g_buff);
	NwUIMain::Get()->SetVertexCount(g_pFbxModel->GetMeshData()->nVertexCount);
	NwUIMain::Get()->SetControlPointCount(g_pFbxModel->GetControlPointGroup()->GetSize());
	NwUIMain::Get()->SetAnimLength(g_pFbxModel->GetAnimTimeLength());
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_OnDropFileKkb(const char* szFileName)
{
	SoStrCpy(g_buff, sizeof(g_buff), szFileName);

	if (g_pFbxModel)
	{
		delete g_pFbxModel;
		g_pFbxModel = 0;
	}
	if (g_pD3DModelFbx)
	{
		delete g_pD3DModelFbx;
		g_pD3DModelFbx = 0;
	}
	if (g_pKKModel)
	{
		delete g_pKKModel;
		g_pKKModel = 0;
	}
	if (g_pD3DModelKK)
	{
		delete g_pD3DModelKK;
		g_pD3DModelKK = 0;
	}
	//
	g_pKKModel = new StKKModel;
	if (g_pKKModel->LoadKkbFile(g_buff) == false)
	{
		const char* szText = SoStrFmt("加载kkb文件失败 %s", g_buff);
		NwUIMain::Get()->SetLog(szText);
		return;
	}

	SoD3DModelInitParam_KK KKParam;
	KKParam.pKKModel = g_pKKModel;
	g_pD3DModelKK = new SoD3DModelKK;
	g_pD3DModelKK->InitModel(&KKParam);


	//以地表为参照物，把模型缩放到合适的大小。
	const float fTerrainWidth = 10.0f;
	//const float fTerrainHeight = 10.0f;
	const float fAdjustScale = 0.5f;
	SoMathFloat3 kMinPos;
	SoMathFloat3 kMaxPos;
	g_pKKModel->GetMeshBoundingBox(&kMinPos, &kMaxPos);
	float fScale = 0.01f;
	if (SoMath_IsFloatZero(kMaxPos.x - kMinPos.x) == false)
	{
		fScale = fTerrainWidth / (kMaxPos.x - kMinPos.x);
		fScale *= fAdjustScale;
	}

	//模型的最低点位于怎样的高度
	const float fModelRootPosZ = 0.0f;
	float fPosZ = (fModelRootPosZ - kMinPos.z) * fScale;

	float fPosX = 5.0f;
	float fPosY = 5.0f;
	XMMATRIX matRotate = XMMatrixRotationZ(0.0f);
	XMMATRIX matScale = XMMatrixScaling(fScale, fScale, fScale);
	XMMATRIX matTranslate = XMMatrixTranslation(fPosX, fPosY, fPosZ);
	XMMATRIX matWorld = XMMatrixMultiply(matRotate, matScale);
	matWorld = XMMatrixMultiply(matWorld, matTranslate);
	XMFLOAT4X4 kMatWorld;
	XMStoreFloat4x4(&kMatWorld, matWorld);

	g_pD3DModelKK->SetWorldMatrix(&kMatWorld);

	//更新UI
	NwUIMain::Get()->SetLog(szFileName);
	NwUIMain::Get()->SetVertexCount(g_pKKModel->GetModelData()->nVertexCount);
	NwUIMain::Get()->SetControlPointCount(g_pKKModel->GetModelData()->nPosCount);
	float fTimeLen = 0.0f;
	const StKKModelAnimation* pAnim = g_pKKModel->GetAnimByID(1);
	if (pAnim)
	{
		 fTimeLen = (float)pAnim->nTimeLength * 0.001f;
	}
	NwUIMain::Get()->SetAnimLength(fTimeLen);
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_OnDropFileKkf(const char* szFileName)
{
	if (g_pKKModel == NULL)
	{
		return;
	}
	//
	g_pKKModel->ClearAllAnimation();
	g_pKKModel->LoadKkfFile(szFileName);

	NwUIMain::Get()->SetLog(szFileName);
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_OnDropFileImage(const char* szFileName)
{
	if (g_pD3DModelFbx)
	{
		g_pD3DModelFbx->SetTexture(szFileName);
		NwUIMain::Get()->SetLog(szFileName);
	}
	if (g_pD3DModelKK)
	{
		g_pD3DModelKK->SetTexture(szFileName);
		NwUIMain::Get()->SetLog(szFileName);
	}
}
//----------------------------------------------------------------
void SoD3DLogicFlowHelp_ConvertFbx2KK()
{
	bool br = true;

	if (g_pFbxModel == NULL)
	{
		return;
	}

	char buff[512];
	SoStrCpy(buff, sizeof(buff), g_buff);
	const int nPos = SoStrRChr(buff, '.');
	if (nPos == -1)
	{
		return;
	}

	StKKFileKkbWrite* pKkbWrite = NULL;
	StKKFileKkfWrite* pKkfWrite = NULL;

	do 
	{
		buff[nPos+1] = 'k';
		buff[nPos+2] = 'k';
		buff[nPos+3] = 'b';
		buff[nPos+4] = 0;
		pKkbWrite = new StKKFileKkbWrite;
		if (pKkbWrite->WriteKkb(buff, g_pFbxModel) == false)
		{
			br = false;
			break;
		}

		if (g_pFbxModel->GetKeyFrameCount() > 0)
		{
			buff[nPos+1] = 'k';
			buff[nPos+2] = 'k';
			buff[nPos+3] = 'f';
			buff[nPos+4] = 0;
			pKkfWrite = new StKKFileKkfWrite;
			if (pKkfWrite->WriteKkf(buff, g_pFbxModel) == false)
			{
				br = false;
				break;
			}
		}

	} while (0);

	if (pKkbWrite)
	{
		delete pKkbWrite;
	}
	if (pKkfWrite)
	{
		delete pKkfWrite;
	}
}
//----------------------------------------------------------------
