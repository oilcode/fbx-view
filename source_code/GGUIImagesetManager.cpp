//----------------------------------------------------------------
#include "GGUIImagesetManager.h"
#include "GGUIImageset.h"
//----------------------------------------------------------------
GGUIImagesetManager* GGUIImagesetManager::ms_pInstance = 0;
//----------------------------------------------------------------
bool GGUIImagesetManager::CreateUIImagesetManager()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new GGUIImagesetManager;
		if (ms_pInstance && ms_pInstance->InitUIImagesetManager())
		{
			br = true;
		}
		else
		{
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void GGUIImagesetManager::ReleaseUIImagesetManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
GGUIImagesetManager::GGUIImagesetManager()
{

}
//----------------------------------------------------------------
GGUIImagesetManager::~GGUIImagesetManager()
{
	ClearUIImagesetManager();
}
//----------------------------------------------------------------
bool GGUIImagesetManager::InitUIImagesetManager()
{
	if (m_kImagesetArray.InitArray(sizeof(GGUIImageset*), 10, 10) == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void GGUIImagesetManager::ClearUIImagesetManager()
{
	const int nCount = m_kImagesetArray.GetCapacity();
	GGUIImageset* pImageset = 0;
	for (int i = 0; i < nCount; ++i)
	{
		pImageset = GetImagesetByID(i);
		if (pImageset)
		{
			delete pImageset;
		}
	}
	m_kImagesetArray.ClearArray();
	m_kName2IndexMap.clear();
}
//----------------------------------------------------------------
int GGUIImagesetManager::CreateImageset(const stImagesetParam& kParam)
{
	int nImagesetID = GetImagesetIndex(kParam.kName);
	if (nImagesetID != -1)
	{
		SoLogError("GGUIImagesetManager::CreateImageset : kName[%s] is already exist!", kParam.kName.GetValue());
		return nImagesetID;
	}

	GGUIImageset* pImageset = new GGUIImageset;
	if (pImageset == 0)
	{
		return nImagesetID;
	}

	pImageset->InitImageset(kParam.nInitRectCount);
	pImageset->SetD3DTexture(kParam.pD3DTexture);
	nImagesetID = m_kImagesetArray.FillAt(-1, &pImageset);
	m_kName2IndexMap.insert(std::make_pair(kParam.kName, nImagesetID));
	return nImagesetID;
}
//----------------------------------------------------------------
int GGUIImagesetManager::GetImagesetIndex(const SoTinyString& kName)
{
	mapName2Index::iterator it = m_kName2IndexMap.find(kName);
	if (it != m_kName2IndexMap.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
//----------------------------------------------------------------
GGUIImageset* GGUIImagesetManager::GetImagesetByID(int nImagesetID)
{
	void* pElement = m_kImagesetArray.GetAt(nImagesetID);
	if (pElement)
	{
		return (*((GGUIImageset**)pElement));
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
GGUIImageset* GGUIImagesetManager::GetImagesetByName(const SoTinyString& kName)
{
	int nIndex = GetImagesetIndex(kName);
	if (nIndex != -1)
	{
		return GetImagesetByID(nIndex);
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
