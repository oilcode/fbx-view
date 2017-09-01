//----------------------------------------------------------------
#include "SoD3DModelManager.h"
//----------------------------------------------------------------
SoD3DModelManager* SoD3DModelManager::ms_pInstance = NULL;
//----------------------------------------------------------------
bool SoD3DModelManager::CreateD3DModelManager()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new SoD3DModelManager;
		if (ms_pInstance && ms_pInstance->InitD3DModelManager())
		{
			br = true;
		}
		else
		{
			ReleaseD3DModelManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void SoD3DModelManager::ReleaseD3DModelManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//----------------------------------------------------------------
SoD3DModelManager::SoD3DModelManager()
{

}
//----------------------------------------------------------------
SoD3DModelManager::~SoD3DModelManager()
{
	ClearD3DModelManager();
}
//----------------------------------------------------------------
bool SoD3DModelManager::InitD3DModelManager()
{
	if (m_kModelArray.InitArray(sizeof(SoD3DModelBase*), 10, 10) == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void SoD3DModelManager::ClearD3DModelManager()
{
	SoD3DModelBase* pModel = 0;
	const int nModelCount = m_kModelArray.GetCapacity();
	for (int i = 0; i < nModelCount; ++i)
	{
		pModel = GetModel(i);
		if (pModel)
		{
			DoDeleteModel(pModel);
		}
	}
	m_kModelArray.ClearArray();
}
//----------------------------------------------------------------
SoD3DModelBase* SoD3DModelManager::CreateModel(int theType, void* pInitParam)
{
	SoD3DModelBase* pModel = NULL;
	switch (theType)
	{
		case SoD3DModelType_Cube:
		{
			//pModel = new SoD3DModelCube;
		} break;
		default:
		{
			SoLogError("SoD3DModelManager::CreateModel : Unknown ModelType [%d]", theType);
		} break;
	}

	if (pModel)
	{
		if (pModel->InitModel(pInitParam))
		{
			pModel->SetModelType((SoD3DModelType)theType);
			const int nModelID = m_kModelArray.FillAt(-1, &pModel);
			pModel->SetModelID(nModelID);
		}
		else
		{
			DoDeleteModel(pModel);
			pModel = NULL;
		}
	}
	return pModel;
}
//----------------------------------------------------------------
void SoD3DModelManager::DeleteModel(int nModelID)
{
	SoD3DModelBase* pModel = GetModel(nModelID);
	if (pModel)
	{
		DoDeleteModel(pModel);
		m_kModelArray.ClearAt(nModelID);
	}
}
//----------------------------------------------------------------
void SoD3DModelManager::DoDeleteModel(SoD3DModelBase* pModel)
{
	if (pModel)
	{
		pModel->ClearModel();
		delete pModel;
		pModel = 0;
	}
}
//----------------------------------------------------------------
SoD3DModelBase* SoD3DModelManager::GetModel(int nModelID)
{
	void* pElement = m_kModelArray.GetAt(nModelID);
	if (pElement)
	{
		return *((SoD3DModelBase**)pElement);
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
