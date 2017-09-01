//----------------------------------------------------------------
#include "GGUIInputHelp.h"
#include "GGUIDefine.h"
#include "WinInputMsg.h"
#include "GGUIWindowFactory.h"
#include "GGUIPanel.h"
#include "GGUIEvent.h"
#include "GGUIWindowContainer.h"
#include "GGUIWindowText.h"
#include "GGUIWindowImage.h"
#include "GGUIWindowButton.h"
//----------------------------------------------------------------
GGUIPanel* g_pCurrentPanel = 0;
//----------------------------------------------------------------
void GGUIInputProcess(stInputEvent* pInputEvent)
{
	if (pInputEvent == 0)
	{
		return;
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	bool bIsUIListenEvent = false;
	if (pInputEvent->theEvent == InputEvent_Down || pInputEvent->theEvent == InputEvent_Up)
	{
		if (pInputEvent->theKey == InputKey_LMouse || pInputEvent->theKey == InputKey_MMouse || pInputEvent->theKey == InputKey_RMouse)
		{
			bIsUIListenEvent = true;
		}
	}
	else if (pInputEvent->theEvent == InputEvent_MouseMove)
	{
		bIsUIListenEvent = true;
	}
	else if (pInputEvent->theEvent == InputEvent_MouseWheel)
	{
		bIsUIListenEvent = true;
	}
	if (bIsUIListenEvent == false)
	{
		return;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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
				g_pCurrentPanel = pPanel;
				GGUIInput_UIContainer(pPanel->GetUIContainer(), pInputEvent);
			}
			//
			if (pInputEvent->bSwallowed == true)
			{
				break;
			}
		}
		//
		if (pInputEvent->bSwallowed == true)
		{
			break;
		}
	}
	g_pCurrentPanel = 0;
}
//----------------------------------------------------------------
void GGUIInput_UIContainer(const GGUIWindowContainer* pUIContainer, stInputEvent* pInputEvent)
{
	GGUIWindowBase* pWindow = 0;
	const int nChildCount = pUIContainer->GetChildCount();
	for (int i = nChildCount-1; i >= 0; --i)
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
		//
		const int nType = pWindow->GetType();
		switch (nType)
		{
		case GGUIWindow_Container:
			{
				GGUIWindowContainer* pContainer = (GGUIWindowContainer*)pWindow;
				GGUIInput_UIContainer(pContainer, pInputEvent);
			} break;
		case GGUIWindow_Text:
			{
				if (pWindow->GetInputEnable())
				{
					GGUIWindowText* pText = (GGUIWindowText*)pWindow;
					GGUIInput_UIText(pText, pInputEvent);
				}
			} break;
		case GGUIWindow_Image:
			{
				if (pWindow->GetInputEnable())
				{
					GGUIWindowImage* pImage = (GGUIWindowImage*)pWindow;
					GGUIInput_UIImage(pImage, pInputEvent);
				}
			} break;
		case GGUIWindow_Button:
			{
				if (pWindow->GetInputEnable())
				{
					GGUIWindowButton* pButton = (GGUIWindowButton*)pWindow;
					GGUIInput_UIButton(pButton, pInputEvent);
				}
			} break;
		default:
			{
				SoLogError("GGUIInput_UIContainer : Unknown WindowType [%d]", nType);
			} break;
		}
		//
		if (pInputEvent->bSwallowed == true)
		{
			break;
		}
	}
	//
	if (pInputEvent->bSwallowed == false)
	{
		if (pUIContainer->GetInputEnable() == true)
		{
			const GGUIRect& kContainerRect = pUIContainer->GetRectInAbsCoord();
			const bool bIsInside = kContainerRect.IsInside((float)pInputEvent->nParam1, (float)pInputEvent->nParam2);
			if (bIsInside)
			{
				pInputEvent->Swallow();
			}
		}
	}
}
//----------------------------------------------------------------
void GGUIInput_UIText(GGUIWindowText* pUIText, stInputEvent* pInputEvent)
{
	//
	const GGUIRect& kRect = pUIText->GetRectInAbsCoord();
	const bool bIsInside = kRect.IsInside((float)pInputEvent->nParam1, (float)pInputEvent->nParam2);
	if (bIsInside)
	{
		pInputEvent->Swallow();
	}
	//
	bool bShouldSendEvent_WindowRect = false;
	if (pUIText->GetCursorInWindowRect() != bIsInside)
	{
		pUIText->SetCursorInWindowRect(bIsInside);
		bShouldSendEvent_WindowRect = true;
	}
	//
	if (bShouldSendEvent_WindowRect)
	{
		if (bIsInside)
		{
			GGUIEventParam_WindowRectEnter kParam;
			kParam.szWindowName = pUIText->GetName().GetValue();
			kParam.nWindowID = pUIText->GetID();
			g_pCurrentPanel->ProcessUIEvent(GGUIEvent_WindowRectEnter, &kParam);
		}
		else
		{
			GGUIEventParam_WindowRectLeave kParam;
			kParam.szWindowName = pUIText->GetName().GetValue();
			kParam.nWindowID = pUIText->GetID();
			g_pCurrentPanel->ProcessUIEvent(GGUIEvent_WindowRectLeave, &kParam);
		}
	}
}
//----------------------------------------------------------------
void GGUIInput_UIImage(GGUIWindowImage* pUIImage, stInputEvent* pInputEvent)
{
	//
	const GGUIRect& kRect = pUIImage->GetRectInAbsCoord();
	const bool bIsInside = kRect.IsInside((float)pInputEvent->nParam1, (float)pInputEvent->nParam2);
	if (bIsInside)
	{
		pInputEvent->Swallow();
	}
	//
	bool bShouldSendEvent_WindowRect = false;
	if (pUIImage->GetCursorInWindowRect() != bIsInside)
	{
		pUIImage->SetCursorInWindowRect(bIsInside);
		bShouldSendEvent_WindowRect = true;
	}
	//
	if (bShouldSendEvent_WindowRect)
	{
		if (bIsInside)
		{
			GGUIEventParam_WindowRectEnter kParam;
			kParam.szWindowName = pUIImage->GetName().GetValue();
			kParam.nWindowID = pUIImage->GetID();
			g_pCurrentPanel->ProcessUIEvent(GGUIEvent_WindowRectEnter, &kParam);
		}
		else
		{
			GGUIEventParam_WindowRectLeave kParam;
			kParam.szWindowName = pUIImage->GetName().GetValue();
			kParam.nWindowID = pUIImage->GetID();
			g_pCurrentPanel->ProcessUIEvent(GGUIEvent_WindowRectLeave, &kParam);
		}
	}
}
//----------------------------------------------------------------
void GGUIInput_UIButton(GGUIWindowButton* pUIButton, stInputEvent* pInputEvent)
{
	//
	const GGUIRect& kRect = pUIButton->GetRectInAbsCoord();
	const bool bIsInside = kRect.IsInside((float)pInputEvent->nParam1, (float)pInputEvent->nParam2);
	if (bIsInside)
	{
		pInputEvent->Swallow();
	}
	//
	bool bShouldSendEvent_WindowRect = false;
	if (pUIButton->GetCursorInWindowRect() != bIsInside)
	{
		pUIButton->SetCursorInWindowRect(bIsInside);
		bShouldSendEvent_WindowRect = true;
	}
	//
	bool bShouldSendEvent_ButtonClick = false;
	const GGUIWindowButton::eButtonState curBtnState = pUIButton->GetButtonState();
	if (pInputEvent->theEvent == InputEvent_MouseMove)
	{
		if (bIsInside)
		{
			if (curBtnState == GGUIWindowButton::ButtonState_Normal)
			{
				pUIButton->SetButtonState(GGUIWindowButton::ButtonState_Hover);
			}
		}
		else
		{
			if (curBtnState != GGUIWindowButton::ButtonState_Normal)
			{
				pUIButton->SetButtonState(GGUIWindowButton::ButtonState_Normal);
			}
		}
	}
	else if (pInputEvent->theEvent == InputEvent_Down && pInputEvent->theKey == InputKey_LMouse)
	{
		if (curBtnState == GGUIWindowButton::ButtonState_Hover)
		{
			pUIButton->SetButtonState(GGUIWindowButton::ButtonState_Push);
		}
	}
	else if (pInputEvent->theEvent == InputEvent_Up && pInputEvent->theKey == InputKey_LMouse)
	{
		if (curBtnState == GGUIWindowButton::ButtonState_Push)
		{
			pUIButton->SetButtonState(GGUIWindowButton::ButtonState_Hover);
			bShouldSendEvent_ButtonClick = true;
		}
	}
	//
	if (bShouldSendEvent_WindowRect)
	{
		if (bIsInside)
		{
			GGUIEventParam_WindowRectEnter kParam;
			kParam.szWindowName = pUIButton->GetName().GetValue();
			kParam.nWindowID = pUIButton->GetID();
			g_pCurrentPanel->ProcessUIEvent(GGUIEvent_WindowRectEnter, &kParam);
		}
		else
		{
			GGUIEventParam_WindowRectLeave kParam;
			kParam.szWindowName = pUIButton->GetName().GetValue();
			kParam.nWindowID = pUIButton->GetID();
			g_pCurrentPanel->ProcessUIEvent(GGUIEvent_WindowRectLeave, &kParam);
		}
	}
	if (bShouldSendEvent_ButtonClick)
	{
		GGUIEventParam_ButtonClick kParam;
		kParam.szWindowName = pUIButton->GetName().GetValue();
		kParam.nWindowID = pUIButton->GetID();
		g_pCurrentPanel->ProcessUIEvent(GGUIEvent_ButtonClick, &kParam);
	}
}
//----------------------------------------------------------------
