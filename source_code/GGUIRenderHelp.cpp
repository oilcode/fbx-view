//----------------------------------------------------------------
#include "GGUIRenderHelp.h"
#include "SoStringHelp.h"
#include "SoStringConvert.h"
#include "SoD3DTextureSystemFont.h"
#include "GGUIRenderManager.h"
#include "GGUIImagesetManager.h"
#include "GGUIImageset.h"
#include "GGUIWindowFactory.h"
#include "GGUIPanel.h"
#include "GGUIWindowContainer.h"
#include "GGUIWindowText.h"
#include "GGUIWindowImage.h"
#include "GGUIWindowButton.h"
#include "GGUIComponentText.h"
#include "NwUIDefine.h"
//----------------------------------------------------------------
stUIRenderUnit g_kUnit;
//----------------------------------------------------------------
void GGUIRenderProcess()
{
	GGUIWindowFactory* pFactory = GGUIWindowFactory::Get();
	if (pFactory == 0)
	{
		return;
	}
	//
	for (int i = 0; i < GGUIPanelSpace_Max; ++i)
	{
		if (pFactory->UISpace_GetVisible(i) == false)
		{
			continue;
		}
		GGUIPanel* pPanel = 0;
		const int nPanelCount = pFactory->UISpace_GetPanelCount(i);
		for (int k = 0; k < nPanelCount; ++k)
		{
			pPanel = pFactory->UISpace_GetPanel(i, k);
			if (pPanel && pPanel->IsPanelVisible())
			{
				GGUIRender_UIContainer(pPanel->GetUIContainer());
			}
		}
	}
}
//----------------------------------------------------------------
void GGUIRender_UIContainer(const GGUIWindowContainer* pUIContainer)
{
	GGUIWindowBase* pWindow = 0;
	const int nChildCount = pUIContainer->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		pWindow = pUIContainer->GetChildByIndex(i);
		if (pWindow == 0)
		{
			continue;
		}
		if (pWindow->GetVisible() == false)
		{
			continue;
		}
		const int nType = pWindow->GetType();
		switch (nType)
		{
			case GGUIWindow_Container:
			{
				GGUIWindowContainer* pContainer = (GGUIWindowContainer*)pWindow;
				GGUIRender_UIContainer(pContainer);
			} break;
			case GGUIWindow_Text:
			{
				GGUIWindowText* pText = (GGUIWindowText*)pWindow;
				GGUIRender_UIText(pText);
			} break;
			case GGUIWindow_Image:
			{
				GGUIWindowImage* pImage = (GGUIWindowImage*)pWindow;
				GGUIRender_UIImage(pImage);
			} break;
			case GGUIWindow_Button:
			{
				GGUIWindowButton* pButton = (GGUIWindowButton*)pWindow;
				GGUIRender_UIButton(pButton);
			} break;
			default:
			{
				SoLogError("GGUIRender_UIContainer : Unknown WindowType [%d]", nType);
			} break;
		} //switch
	}
}
//----------------------------------------------------------------
void GGUIRender_SimpleText(const char* szText, const GGUIRect& kRect, const SoTinyString& kFont, const GGUIColor& kColor)
{
	if (szText == 0 || szText[0] == 0)
	{
		return;
	}
	const GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByName(kFont);
	if (pImageset == 0)
	{
		return;
	}
	wchar_t* wString = SoAnsiToUnicode(szText);
	if (wString[0] == 0)
	{
		return;
	}
	//
	SoTinyString kRectName;
	wchar_t wRectName[2] = {0};
	float fCurRectLeft = kRect.x;
	float fCurRectTop = kRect.y;
	const float fTexWidth = pImageset->GetTextureWidth();
	const float fTexHeight = pImageset->GetTextureHeight();
	const int nWCharCount = SoWStrLen(wString);
	for (int i = 0; i < nWCharCount; ++i)
	{
		wRectName[0] = wString[i];
		kRectName = (char*)wRectName;
		int nRectIndex = pImageset->GetRectID(kRectName);
		if (nRectIndex == -1)
		{
			SoD3DTextureSystemFont::Get()->DrawString(&(wRectName[0]), 1);
			nRectIndex = pImageset->GetRectID(kRectName);
		}
		if (nRectIndex == -1)
		{
			continue;
		}
		//
		const GGUIRect& imageRect = pImageset->GetRect(nRectIndex);
		g_kUnit.fRectLeft = fCurRectLeft;
		g_kUnit.fRectTop = fCurRectTop;
		g_kUnit.fRectWidth = imageRect.w * fTexWidth;
		g_kUnit.fRectHeight = imageRect.h * fTexHeight;
		g_kUnit.fTexCoordLeft = imageRect.x;
		g_kUnit.fTexCoordTop = imageRect.y;
		g_kUnit.fTexCoordWidth = imageRect.w;
		g_kUnit.fTexCoordHeight = imageRect.h;
		g_kUnit.pTexture = pImageset->GetTexture();
		g_kUnit.fColorR = kColor.r;
		g_kUnit.fColorG = kColor.g;
		g_kUnit.fColorB = kColor.b;
		g_kUnit.fColorA = kColor.a;
		GGUIRenderManager::Get()->AddRnederUnit(&g_kUnit);
		//
		fCurRectLeft += g_kUnit.fRectWidth;
	}
}
//----------------------------------------------------------------
void GGUIRender_UIText(const GGUIWindowText* pUIText)
{
	GGUIRender_SimpleText(pUIText->GetText().c_str(), pUIText->GetRectInAbsCoord(), pUIText->GetFont(), pUIText->GetColor());
}
//----------------------------------------------------------------
void GGUIRender_UIImage(const GGUIWindowImage* pUIImage)
{
	const int nImagesetIndex = pUIImage->GetImagesetIndex();
	if (nImagesetIndex == -1)
	{
		return;
	}
	const int nRectIndex = pUIImage->GetImageRectIndex();
	if (nRectIndex == -1)
	{
		return;
	}
	const GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(nImagesetIndex);
	if (pImageset == 0)
	{
		return;
	}
	//
	const GGUIRect& kRect = pUIImage->GetRectInAbsCoord();
	const GGUIRect& imageRect = pImageset->GetRect(nRectIndex);
	g_kUnit.fRectLeft = kRect.x;
	g_kUnit.fRectTop = kRect.y;
	g_kUnit.fRectWidth = kRect.w;
	g_kUnit.fRectHeight = kRect.h;
	g_kUnit.fTexCoordLeft = imageRect.x;
	g_kUnit.fTexCoordTop = imageRect.y;
	g_kUnit.fTexCoordWidth = imageRect.w;
	g_kUnit.fTexCoordHeight = imageRect.h;
	g_kUnit.pTexture = pImageset->GetTexture();
	g_kUnit.fColorR = 0.0f;
	g_kUnit.fColorG = 0.0f;
	g_kUnit.fColorB = 0.0f;
	g_kUnit.fColorA = 0.0f;
	GGUIRenderManager::Get()->AddRnederUnit(&g_kUnit);
}
//----------------------------------------------------------------
void GGUIRender_UIButton(const GGUIWindowButton* pUIButton)
{
	const int nImagesetIndex = pUIButton->GetImagesetIndex();
	if (nImagesetIndex == -1)
	{
		return;
	}
	const int nRectIndex = pUIButton->GetImageRectIndex();
	if (nRectIndex == -1)
	{
		return;
	}
	const GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(nImagesetIndex);
	if (pImageset == 0)
	{
		return;
	}
	//
	float fDeltaX_BtnState = 0.0f;
	float fDeltaY_BtnState = 0.0f;
	float fColorR_BtnState = 0.0f;
	float fColorG_BtnState = 0.0f;
	float fColorB_BtnState = 0.0f;
	float fColorA_BtnState = 0.0f;
	const GGUIWindowButton::eButtonState curBtnState = pUIButton->GetButtonState();
	if (curBtnState == GGUIWindowButton::ButtonState_Hover)
	{
		fColorR_BtnState = GGUI_ButtonStateHover_ColorR;
		fColorG_BtnState = GGUI_ButtonStateHover_ColorG;
		fColorB_BtnState = GGUI_ButtonStateHover_ColorB;
		fColorA_BtnState = GGUI_ButtonStateHover_ColorA;
		fDeltaX_BtnState = GGUI_ButtonStateHover_DeltaX;
		fDeltaY_BtnState = GGUI_ButtonStateHover_DeltaY;
	}
	else if (curBtnState == GGUIWindowButton::ButtonState_Push)
	{
		fColorR_BtnState = GGUI_ButtonStatePush_ColorR;
		fColorG_BtnState = GGUI_ButtonStatePush_ColorG;
		fColorB_BtnState = GGUI_ButtonStatePush_ColorB;
		fColorA_BtnState = GGUI_ButtonStatePush_ColorA;
		fDeltaX_BtnState = GGUI_ButtonStatePush_DeltaX;
		fDeltaY_BtnState = GGUI_ButtonStatePush_DeltaY;
	}
	//
	const GGUIRect& kRect = pUIButton->GetRectInAbsCoord();
	const GGUIRect& imageRect = pImageset->GetRect(nRectIndex);
	g_kUnit.fRectLeft = kRect.x + fDeltaX_BtnState;
	g_kUnit.fRectTop = kRect.y + fDeltaY_BtnState;
	g_kUnit.fRectWidth = kRect.w;
	g_kUnit.fRectHeight = kRect.h;
	g_kUnit.fTexCoordLeft = imageRect.x;
	g_kUnit.fTexCoordTop = imageRect.y;
	g_kUnit.fTexCoordWidth = imageRect.w;
	g_kUnit.fTexCoordHeight = imageRect.h;
	g_kUnit.pTexture = pImageset->GetTexture();
	g_kUnit.fColorR = fColorR_BtnState;
	g_kUnit.fColorG = fColorG_BtnState;
	g_kUnit.fColorB = fColorB_BtnState;
	g_kUnit.fColorA = fColorA_BtnState;
	GGUIRenderManager::Get()->AddRnederUnit(&g_kUnit);

	GGUIRender_SimpleText(pUIButton->GetText(), pUIButton->GetRectInAbsCoord(), NwUI_DefaultFont, GGUIColor_White);
}
//----------------------------------------------------------------
void GGUIRender_ComponetText(const GGUIComponentText* pCompText)
{
	const int nCount = pCompText->GetTextChunkCount();
	if (nCount == 0)
	{
		return;
	}

	SoTinyString kRectName;
	wchar_t wRectName[2] = {0};
	//
	for (int i = 0; i < nCount; ++i)
	{
		const GGUITextChunk* pChunk = pCompText->GetTextChunk(i);
		if (pChunk == 0)
		{
			continue;
		}
		const GGUIImageset* pImageset = GGUIImagesetManager::Get()->GetImagesetByID(pChunk->FontImagesetID);
		if (pImageset == 0)
		{
			continue;
		}
		//
		const float fTexWidth = pImageset->GetTextureWidth();
		const float fTexHeight = pImageset->GetTextureHeight();
		//
		float fCurRectLeft = pChunk->kRect.x;
		float fCurRectTop = pChunk->kRect.y;
		const int nCharCount = pChunk->TextCount;
		for (int j = 0; j < nCharCount; ++j)
		{
			wRectName[0] = pChunk->szText[j];
			kRectName = (char*)wRectName;
			const int nRectIndex = pImageset->GetRectID(kRectName);
			if (nRectIndex == -1)
			{
				continue;
			}
			//
			const GGUIRect& imageRect = pImageset->GetRect(nRectIndex);
			g_kUnit.fRectLeft = fCurRectLeft;
			g_kUnit.fRectTop = fCurRectTop;
			g_kUnit.fRectWidth = imageRect.w * fTexWidth;
			g_kUnit.fRectHeight = imageRect.h * fTexHeight;
			g_kUnit.fTexCoordLeft = imageRect.x;
			g_kUnit.fTexCoordTop = imageRect.y;
			g_kUnit.fTexCoordWidth = imageRect.w;
			g_kUnit.fTexCoordHeight = imageRect.h;
			g_kUnit.pTexture = pImageset->GetTexture();
			g_kUnit.fColorR = pChunk->kColor.r;
			g_kUnit.fColorG = pChunk->kColor.g;
			g_kUnit.fColorB = pChunk->kColor.b;
			g_kUnit.fColorA = pChunk->kColor.a;
			GGUIRenderManager::Get()->AddRnederUnit(&g_kUnit);
			//
			fCurRectLeft += g_kUnit.fRectWidth;
		}
	}
}
//----------------------------------------------------------------
