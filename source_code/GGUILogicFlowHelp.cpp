//----------------------------------------------------------------
#include "GGUILogicFlowHelp.h"
#include "GGUIRenderManager.h"
#include "GGUIImagesetManager.h"
#include "SoD3DSystem.h"
#include "SoD3DTextureSystemFont.h"
#include "GGUIWindowFactory.h"
#include "GGUIRenderHelp.h"
#include "WinApp.h"
#include "GGUIRichTextDefine.h"
#include "GGUIComponentText.h"
//----------------------------------------------------------------
//wchar_t rrr[200] = L"<br><br>  < anchor=xcenter; anchor =top ;color=ffff00ff>梅<dfe><br><color=ff00ffff>梅<color=00ffffff>你好吗<font=宋体>bbbbbb<color= 81009999>晚上<br>回家";
//wchar_t kkk[200] = L"<color = 0000ffff >减<br>少所示<color=ffffff77>透明";
//GGUIComponentText kCompText;
//----------------------------------------------------------------
bool GGUILogicFlowHelp_Create()
{
	if (WinApp::Get())
	{
		GGUIFunc_OnResolutionChanged((float)WinApp::Get()->GetClientWidth(), (float)WinApp::Get()->GetClientHeight());
	}
	if (GGUIRenderManager::CreateUIRenderManager() == false)
	{
		return false;
	}
	if (GGUIImagesetManager::CreateUIImagesetManager() == false)
	{
		return false;
	}
	if (SoD3DTextureSystemFont::CreateD3DTextureSystemFont() == false)
	{
		return false;
	}
	if (GGUIWindowFactory::CreateUIWindowFactory() == false)
	{
		return false;
	}

	//<<<<<<<<<<<<<<<<<<<<<<<<
	//kCompText.SetMaxWidth(88.0f);
	//kCompText.SetMaxHeight(500.0f);
	//GGUIRichText_Pause(rrr, &kCompText);
	//GGUIRichText_Pause(kkk, &kCompText);
	//>>>>>>>>>>>>>>>>>>>>>>>>

	return true;
}
//----------------------------------------------------------------
void GGUILogicFlowHelp_Release()
{
	GGUIWindowFactory::ReleaseUIWindowFactory();
	SoD3DTextureSystemFont::ReleaseD3DTextureSystemFont();
	GGUIImagesetManager::ReleaseUIImagesetManager();
	GGUIRenderManager::ReleaseUIRenderManager();
}
//----------------------------------------------------------------
void GGUILogicFlowHelp_Update(float fDeltaTime)
{

}
//----------------------------------------------------------------
void GGUILogicFlowHelp_PreRender()
{
	if (SoD3DTextureSystemFont::Get())
	{
		SoD3DTextureSystemFont::Get()->PreRenderD3DTextureSystemFont();
	}
}
//----------------------------------------------------------------
void GGUILogicFlowHelp_Render()
{
	GGUIRenderProcess();
	//GGUIRender_ComponetText(&kCompText);
	//
	if (GGUIRenderManager::Get())
	{
		SoD3DSystem::Get()->ClearDepthStencil();
		GGUIRenderManager::Get()->RenderUIRenderManager();
	}
}
//----------------------------------------------------------------
