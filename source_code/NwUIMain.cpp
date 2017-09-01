//----------------------------------------------------------------
#include "NwUIMain.h"
//----------------------------------------------------------------
NwUIMain* NwUIMain::ms_pInstance = 0;
//----------------------------------------------------------------
bool NwUIMain::CreateUIMain()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new NwUIMain;
		if (ms_pInstance && ms_pInstance->InitUIMain())
		{
			br = true;
		}
		else
		{
			ReleaseUIMain();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void NwUIMain::ReleaseUIMain()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
NwUIMain::NwUIMain()
:m_pTextLog(0)
,m_pUITextFPS(0)
,m_pTextVertexCount(0)
,m_pTextControlPointCount(0)
,m_pTextAnimLength(0)
,m_pBtnConvert(0)
,m_fAccTimeForFPS(0.0f)
{

}
//----------------------------------------------------------------
NwUIMain::~NwUIMain()
{
	ClearUIMain();
}
//----------------------------------------------------------------
bool NwUIMain::InitUIMain()
{
	GGUIFullRect kFullRect;
	kFullRect.fScaleX = 0.0f;
	kFullRect.fDeltaX = 2.0f;
	kFullRect.fScaleY = 0.0f;
	kFullRect.fDeltaY = 10.0f;
	kFullRect.fScaleW = 1.0f;
	kFullRect.fDeltaW = 0.0f;
	kFullRect.fScaleH = 0.0f;
	kFullRect.fDeltaH = 20.0f;
	GGUIWindowText* pTextLog = (GGUIWindowText*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Text);
	pTextLog->SetFullRect(kFullRect);
	pTextLog->SetColor(GGUIColor_Green);
	pTextLog->SetFont(NwUI_DefaultFont);
	pTextLog->SetText("");


	kFullRect.fScaleX = 1.0f;
	kFullRect.fDeltaX = -100.0f;
	kFullRect.fScaleY = 0.0f;
	kFullRect.fDeltaY = 10.0f;
	kFullRect.fScaleW = 0.0f;
	kFullRect.fDeltaW = 100.0f;
	kFullRect.fScaleH = 0.0f;
	kFullRect.fDeltaH = 20.0f;
	GGUIWindowText* pUITextFPS = (GGUIWindowText*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Text);
	pUITextFPS->SetFullRect(kFullRect);
	pUITextFPS->SetColor(GGUIColor_Red);
	pUITextFPS->SetFont(NwUI_DefaultFont);
	pUITextFPS->SetText("FPS:N/V");


	kFullRect.fDeltaX = -200.0f;
	kFullRect.fDeltaY = 30.0f;
	kFullRect.fDeltaW = 100.0f;
	kFullRect.fDeltaH = 20.0f;
	GGUIWindowText* pTextVertexCount = (GGUIWindowText*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Text);
	pTextVertexCount->SetFullRect(kFullRect);
	pTextVertexCount->SetColor(GGUIColor_Red);
	pTextVertexCount->SetFont(NwUI_DefaultFont);
	pTextVertexCount->SetText("VertexCount: 0");

	kFullRect.fDeltaX = -200.0f;
	kFullRect.fDeltaY = 50.0f;
	kFullRect.fDeltaW = 100.0f;
	kFullRect.fDeltaH = 20.0f;
	GGUIWindowText* pTextControlPointCount = (GGUIWindowText*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Text);
	pTextControlPointCount->SetFullRect(kFullRect);
	pTextControlPointCount->SetColor(GGUIColor_Red);
	pTextControlPointCount->SetFont(NwUI_DefaultFont);
	pTextControlPointCount->SetText("ControlPointCount: 0");

	kFullRect.fDeltaX = -200.0f;
	kFullRect.fDeltaY = 70.0f;
	kFullRect.fDeltaW = 100.0f;
	kFullRect.fDeltaH = 20.0f;
	GGUIWindowText* pTextAnimLength = (GGUIWindowText*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Text);
	pTextAnimLength->SetFullRect(kFullRect);
	pTextAnimLength->SetColor(GGUIColor_Red);
	pTextAnimLength->SetFont(NwUI_DefaultFont);
	pTextAnimLength->SetText("AnimLength: 0");

	//kFullRect.fDeltaX = -150.0f;
	//kFullRect.fDeltaY = 100.0f;
	//kFullRect.fDeltaW = 140.0f;
	//kFullRect.fDeltaH = 30.0f;
	//GGUIWindowButton* pUIButton = (GGUIWindowButton*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Button);
	//pUIButton->SetFullRect(kFullRect);
	//pUIButton->SetTexture("texture/mm1:Name1");
	//pUIButton->SetText("°Ñfbx×ª»»³Ékk");


	kFullRect.fDeltaX = 0.0f;
	kFullRect.fDeltaY = 0.0f;
	kFullRect.fDeltaW = GGUIFunc_GetResolutionWidth();
	kFullRect.fDeltaH = GGUIFunc_GetResolutionHeight();
	GGUIWindowContainer* pUIContainer = (GGUIWindowContainer*)GGUIWindowFactory::Get()->CreateUIWindow(GGUIWindow_Container);
	pUIContainer->SetFullRect(kFullRect);
	pUIContainer->SetInputEnable(true);
	pUIContainer->AddChild(pTextLog);
	pUIContainer->AddChild(pUITextFPS);
	pUIContainer->AddChild(pTextVertexCount);
	pUIContainer->AddChild(pTextControlPointCount);
	pUIContainer->AddChild(pTextAnimLength);
	//pUIContainer->AddChild(pUIButton);
	InitPanelByUIContainer(pUIContainer, GGUIPanelSpace_1);

	m_pTextLog = pTextLog;
	m_pUITextFPS = pUITextFPS;
	m_pTextVertexCount = pTextVertexCount;
	m_pTextControlPointCount = pTextControlPointCount;
	m_pTextAnimLength = pTextAnimLength;
	//m_pBtnConvert = pUIButton;

	return true;
}
//----------------------------------------------------------------
void NwUIMain::ClearUIMain()
{
	ClearPanel();
	m_pTextLog = 0;
	m_pUITextFPS = 0;
	m_pTextVertexCount = 0;
	m_pTextControlPointCount = 0;
	m_pTextAnimLength = 0;
	m_pBtnConvert = 0;
}
//----------------------------------------------------------------
void NwUIMain::UpdateUIMain(float fDeltaTime)
{
	m_fAccTimeForFPS += fDeltaTime;
	if (m_fAccTimeForFPS > 1.0f)
	{
		m_fAccTimeForFPS = 0.0f;
		if (m_pUITextFPS)
		{
			const int nFPS = SoGameTime::Get()->GetCurrentFPS();
			const char* szFPS = SoStrFmt("FPS:%d", nFPS);
			m_pUITextFPS->SetText(szFPS);
		}
	}
}
//----------------------------------------------------------------
void NwUIMain::SetLog(const char* szLog)
{
	if (m_pTextLog)
	{
		m_pTextLog->SetText(szLog);
	}
}
//----------------------------------------------------------------
void NwUIMain::SetVertexCount(int nCount)
{
	if (m_pTextVertexCount)
	{
		const char* szText = SoStrFmt("VertexCount: %d", nCount);
		m_pTextVertexCount->SetText(szText);
	}
}
//----------------------------------------------------------------
void NwUIMain::SetControlPointCount(int nCount)
{
	if (m_pTextControlPointCount)
	{
		const char* szText = SoStrFmt("ControlPointCount: %d", nCount);
		m_pTextControlPointCount->SetText(szText);
	}
}
//----------------------------------------------------------------
void NwUIMain::SetAnimLength(float fLen)
{
	if (m_pTextAnimLength)
	{
		const char* szText = SoStrFmt("AnimLength: %.2f", fLen);
		m_pTextAnimLength->SetText(szText);
	}
}
//----------------------------------------------------------------
void NwUIMain::ProcessUIEvent(int nEventType, void* pParam)
{
	static bool s_bFF = true;

	switch (nEventType)
	{
	case GGUIEvent_ButtonClick:
		{
			GGUIEventParam_ButtonClick* myParam = (GGUIEventParam_ButtonClick*)pParam;
			if (myParam->nWindowID == m_pBtnConvert->GetID())
			{
				
			}
		} break;
	}
}
//----------------------------------------------------------------
