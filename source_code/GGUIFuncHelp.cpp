//----------------------------------------------------------------
#include "GGUIDefine.h"
#include "GGUIImagesetManager.h"
#include "GGUIImageset.h"
#include "GGUIWindowFactory.h"
#include "GGUIPanel.h"
//----------------------------------------------------------------
float g_fResolutionWidth = 800.0f;
float g_fResolutionHeight = 600.0f;
//----------------------------------------------------------------
bool GGUIFunc_GetImagesetIndexRectIndex(const char* szTexture, int& nImagesetIndex, int& nRectIndex)
{
	if (szTexture == 0 || szTexture[0] == 0)
	{
		return false;
	}
	const int nSplitIndex = SoStrChr(szTexture, ':');
	if (nSplitIndex == -1)
	{
		return false;
	}
	const char* pImagesetName = SoSubStr(szTexture, 0, nSplitIndex-1);
	SoTinyString kName(pImagesetName);
	nImagesetIndex = GGUIImagesetManager::Get()->GetImagesetIndex(kName);
	if (nImagesetIndex == -1)
	{
		return false;
	}
	GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(nImagesetIndex);
	if (pImageset)
	{
		kName.SetValue(szTexture + nSplitIndex + 1);
		nRectIndex = pImageset->GetRectID(kName);
	}
	return true;
}
//----------------------------------------------------------------
void GGUIFunc_UIntConvertToGGUIColor(unsigned int uiColor, GGUIColor* pColor)
{
	const float fScale = 1.0f / 255.0f;

	unsigned int uiR = uiColor >> 24;
	pColor->r = ((float)uiR) * fScale;

	unsigned int uiG = uiColor << 8;
	uiG = uiG >> 24;
	pColor->g = ((float)uiG) * fScale;

	unsigned int uiB = uiColor << 16;
	uiB = uiB >> 24;
	pColor->b = ((float)uiB) * fScale;

	unsigned int uiA = uiColor << 24;
	uiA = uiA >> 24;
	pColor->a = ((float)uiA) * fScale;
}
//----------------------------------------------------------------
void GGUIFunc_OnResolutionChanged(float newWidth, float newHeight)
{
	g_fResolutionWidth = newWidth;
	g_fResolutionHeight = newHeight;
	//
	GGUIWindowFactory* pFactory = GGUIWindowFactory::Get();
	if (pFactory == 0)
	{
		return;
	}
	//
	for (int i = 0; i < GGUIPanelSpace_Max; ++i)
	{
		GGUIPanel* pPanel = 0;
		const int nPanelCount = pFactory->UISpace_GetPanelCount(i);
		for (int k = 0; k < nPanelCount; ++k)
		{
			pPanel = pFactory->UISpace_GetPanel(i, k);
			if (pPanel)
			{
				pPanel->OnResolutionChanged(newWidth, newHeight);
			}
		}
	}
}
//----------------------------------------------------------------
float GGUIFunc_GetResolutionWidth()
{
	return g_fResolutionWidth;
}
//----------------------------------------------------------------
float GGUIFunc_GetResolutionHeight()
{
	return g_fResolutionHeight;
}
//----------------------------------------------------------------
