//----------------------------------------------------------------
#include "NwUILogicFlowHelp.h"
#include "NwUIMain.h"
#include "GGUIImagesetIO.h"
//----------------------------------------------------------------
bool NwUILogicFlowHelp_Create()
{
	GGUIImagesetIO::Read("texture/mm1.txt");
	//
	if (NwUIMain::CreateUIMain() == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void NwUILogicFlowHelp_Release()
{
	NwUIMain::ReleaseUIMain();
}
//----------------------------------------------------------------
void NwUILogicFlowHelp_Update(float fDeltaTime)
{
	if (NwUIMain::Get())
	{
		NwUIMain::Get()->UpdateUIMain(fDeltaTime);
	}
}
//----------------------------------------------------------------
