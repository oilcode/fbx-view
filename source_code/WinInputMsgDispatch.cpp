//----------------------------------------------------------------
#include "WinInputMsgDispatch.h"
#include "WinInputMsg.h"
#include "SoD3DCamera.h"
#include "GGUIInputHelp.h"
//----------------------------------------------------------------
void WinInputMsgDispatch_Process(stInputEvent* pInputEvent)
{
	if (SoD3DCamera::Get())
	{
		SoD3DCamera::Get()->ProcessInputEvent(pInputEvent);
		if (pInputEvent->bSwallowed == true)
		{
			return;
		}
	}
	//
	GGUIInputProcess(pInputEvent);
	if (pInputEvent->bSwallowed == true)
	{
		return;
	}
}
//----------------------------------------------------------------
