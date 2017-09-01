//----------------------------------------------------------------
#include "GGUIImagesetIO.h"
#include "GGUIDefine.h"
#include "GGUIFileGGM.h"
#include "GGUIImagesetManager.h"
#include "GGUIImageset.h"
#include "SoD3DTextureManager.h"
//----------------------------------------------------------------
bool GGUIImagesetIO::Read(const char* szImagesetName)
{
	if (szImagesetName == 0 || szImagesetName[0] == 0)
	{
		return false;
	}
	if (GGUIImagesetManager::Get() == 0)
	{
		return false;
	}
	if (SoD3DTextureManager::Get() == 0)
	{
		return false;
	}

	GGUIFileGGM kFile;
	if (kFile.ReadFcfFile(szImagesetName) == false)
	{
		return false;
	}

	const int nRectCount = kFile.GetImageRectCount();
	if (nRectCount == 0)
	{
		return false;
	}

	std::string strTextureName;
	if (kFile.GetTextureName(strTextureName) == false)
	{
		return false;
	}

	SoD3DTexture* pTexture = SoD3DTextureManager::Get()->CreateUITextureFromFile(strTextureName.c_str());
	if (pTexture == 0)
	{
		return false;
	}

	stImagesetParam kParam;
	const int nDotIndex = SoStrRChr(szImagesetName, '.');
	kParam.kName = SoSubStr(szImagesetName, 0, nDotIndex-1);
	kParam.nInitRectCount = nRectCount;
	kParam.pD3DTexture = pTexture;

	const int nImagesetIndex = GGUIImagesetManager::Get()->CreateImageset(kParam);
	if (nImagesetIndex == -1)
	{
		return false;
	}

	GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(nImagesetIndex);
	if (pImageset == 0)
	{
		return false;
	}

	SoTinyString kRectName;
	GGUIRect kRectData;
	for (int i = 0; i < nRectCount; ++i)
	{
		if (kFile.GetNextImageRect(kRectName, kRectData))
		{
			pImageset->AddRect(kRectName, kRectData);
		}
		else
		{
			break;
		}
	}

	return true;
}
//----------------------------------------------------------------
