//-----------------------------------------------------------------------
//1，摄像机的世界矩阵，最基本的输入是m_kPosition，m_fPitch，m_fYaw三个元素，
//   根据这三个元素可以计算得到三个轴向量，然后就可以得到世界矩阵。
//2，摄像机的旋转：
//   CameraMode_Free模式：响应鼠标右键按下并拖动的消息，然后做出相应的旋转。
//   CameraMode_Focus模式：响应鼠标右键按下并拖动的消息，然后做出相应的旋转。最后把摄像机的轴向量转换成客户端主角的轴向量。
//3，摄像机的位置：
//   CameraMode_Free模式：判断方向键是否处于按下状态，然后做出相应的移动。
//   CameraMode_Focus模式：先获取客户端主角的位置，然后根据位置关系计算摄像机的坐标。
//-----------------------------------------------------------------------
#include "SoD3DCamera.h"
#include "SoCursor.h"
//-----------------------------------------------------------------------
SoD3DCamera* SoD3DCamera::ms_pInstance = NULL;
//-----------------------------------------------------------------------
bool SoD3DCamera::CreateD3DCamera()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new SoD3DCamera;
		if (ms_pInstance && ms_pInstance->InitD3DCamera())
		{
			//什么都不做
		}
		else
		{
			ReleaseD3DCamera();
			br = false;
		}
	}
	return false;
}
//-----------------------------------------------------------------------
void SoD3DCamera::ReleaseD3DCamera()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//-----------------------------------------------------------------------
SoD3DCamera::SoD3DCamera()
:m_CameraMode(CameraMode_Free)
,m_nMouseRightKeyDown(0)
,m_nLastCursorPosX(0)
,m_nLastCursorPosY(0)
,m_nLastMousePosX(0)
,m_nLastMousePosY(0)
,m_kFocusPos(0.0f, 0.0f, 0.0f)
,m_fFocusDistance(8.0f)
{

}
//-----------------------------------------------------------------------
SoD3DCamera::~SoD3DCamera()
{
	ClearD3DCamera();
}
//-----------------------------------------------------------------------
bool SoD3DCamera::InitD3DCamera()
{
	const float width = (float)(WinApp::Get()->GetClientWidth());
	const float height = (float)(WinApp::Get()->GetClientHeight());
	SetFrustum(0.25f*SoMath_PI, width/height, 1.0f, 10000.0f);
	//
	SetPosition(5.0f, -4.0f, 6.0f);
	SetPitchYaw(-0.6f, -0.01f);
	return true;
}
//-----------------------------------------------------------------------
void SoD3DCamera::ClearD3DCamera()
{
	// do nothing
}
//-----------------------------------------------------------------------
void SoD3DCamera::ProcessInputEvent(stInputEvent* pInputEvent)
{
	if (pInputEvent->theEvent == InputEvent_Down && pInputEvent->theKey == InputKey_RMouse)
	{
		if (m_nMouseRightKeyDown == 0)
		{
			pInputEvent->Swallow();
			m_nMouseRightKeyDown = 1;
			m_nLastMousePosX = pInputEvent->nParam1;
			m_nLastMousePosY = pInputEvent->nParam2;
			WinInputMsgManager::Get()->SetMouseMoveForCameraOp(true);
			if (SoCursor::Get())
			{
				SoCursor::Get()->SetCursorVisible(false);
				SoCursor::Get()->SetCursorCapture(true);
				SoCursor::Get()->GetCursorPosition(m_nLastCursorPosX, m_nLastCursorPosY);
			}
		}
	}
	else if (pInputEvent->theEvent == InputEvent_Up && pInputEvent->theKey == InputKey_RMouse)
	{
		if (m_nMouseRightKeyDown == 1)
		{
			pInputEvent->Swallow();
			m_nMouseRightKeyDown = 0;
			m_nLastMousePosX = 0;
			m_nLastMousePosY = 0;
			WinInputMsgManager::Get()->SetMouseMoveForCameraOp(false);
			if (SoCursor::Get())
			{
				SoCursor::Get()->SetCursorVisible(true);
				SoCursor::Get()->SetCursorCapture(false);
				SoCursor::Get()->SetCursorPosition(m_nLastCursorPosX, m_nLastCursorPosY);
				m_nLastCursorPosX = 0;
				m_nLastCursorPosY = 0;
			}
		}
	}
	else if (pInputEvent->theEvent == InputEvent_MouseMove_CameraOp && pInputEvent->theKey == InputKey_RMouse)
	{
		if (m_nMouseRightKeyDown)
		{
			pInputEvent->Swallow();
			float fDeltaX = (float)(pInputEvent->nParam1 - m_nLastMousePosX);
			float fDeltaY = (float)(pInputEvent->nParam2 - m_nLastMousePosY);
			//为了实现这样的效果，“旋转前，旋转过程中和旋转后，光标始终都位于同一位置，
			//目的是鼠标移动过程中，光标不会因为到达客户端窗口的边界而导致旋转中止”，
			//要做两个步骤：
			//1，m_nLastMousePosX和m_nLastMousePosY的值始终保持不变
			//2，总是把光标的位置设置成旋转前的位置
			//m_nLastMousePosX = pInputEvent->nParam1;
			//m_nLastMousePosY = pInputEvent->nParam2;
			SoCursor::Get()->SetCursorPosition(m_nLastCursorPosX, m_nLastCursorPosY);
			//
			//fDeltaX代表了以Up方向为轴的旋转角度。在右手坐标系下，fDeltaX值为负表示旋转角度为正。
			//fDeltaY代表了以Right方向为轴的旋转角度。在右手坐标系下，fDeltaY值为负表示旋转角度为正。
			//位移大小转变成角度变化的大小。
			float fDeltaRadianByUpDir = XMConvertToRadians(-fDeltaX * SoD3D_CameraRotateSpeed);
			float fDeltaRadianByRightDir = XMConvertToRadians(-fDeltaY * SoD3D_CameraRotateSpeed);
			SetDeltaPitchYaw(fDeltaRadianByRightDir, fDeltaRadianByUpDir);
		}
	}
	else if (pInputEvent->theEvent == InputEvent_MouseWheel)
	{
		if (m_CameraMode == CameraMode_Focus)
		{
			pInputEvent->Swallow();
			float fDelta = (float)(pInputEvent->nParam1);
			fDelta *= - SoD3D_CameraFocusDistanceScale;
			SetFocusDeltaDistance(fDelta);
		}
	}
}
//-----------------------------------------------------------------------
void SoD3DCamera::UpdateFreeMove(float fDeltaTime)
{
	if (m_CameraMode == CameraMode_Free)
	{
		float fDeltaX = 0.0f;
		float fDeltaY = 0.0f;
		float fCameraSpeed = SoD3D_CameraMoveSpeed;
		if (WinInputMsgManager::Get()->IsKeyDown(InputKey_A))
		{
			fDeltaX += -fCameraSpeed * fDeltaTime;
		}
		if (WinInputMsgManager::Get()->IsKeyDown(InputKey_D))
		{
			fDeltaX += fCameraSpeed * fDeltaTime;
		}
		if (WinInputMsgManager::Get()->IsKeyDown(InputKey_S))
		{
			fDeltaY += -fCameraSpeed * fDeltaTime;
		}
		if (WinInputMsgManager::Get()->IsKeyDown(InputKey_W))
		{
			fDeltaY += fCameraSpeed * fDeltaTime;
		}
		SetDeltaMove(fDeltaX, fDeltaY);
	}
}
//-----------------------------------------------------------------------
void SoD3DCamera::SetFocusPos(float x, float y, float z)
{
	if (m_CameraMode == CameraMode_Focus)
	{
		m_kFocusPos.x = x;
		m_kFocusPos.y = y;
		m_kFocusPos.z = z;

		//m_kPosition = m_kFocusPos - m_kLook * m_fFocusDistance
		float newPosX = x - m_kLook.x * m_fFocusDistance;
		float newPosY = y - m_kLook.y * m_fFocusDistance;
		float newPosZ = z - m_kLook.z * m_fFocusDistance;
		SetPosition(newPosX, newPosY, newPosZ);
	}
}
//-----------------------------------------------------------------------
void SoD3DCamera::SetFocusDistance(float fDistance)
{
	if (m_CameraMode == CameraMode_Focus)
	{
		SetFocusDeltaDistance(fDistance - m_fFocusDistance);
	}
}
//-----------------------------------------------------------------------
void SoD3DCamera::SetFocusDeltaDistance(float deltaDistance)
{
	if (m_CameraMode == CameraMode_Focus)
	{
		if (SoMath_IsFloatZero(deltaDistance) == false)
		{
			m_fFocusDistance += deltaDistance;

			//m_kPosition = m_kFocusPos - m_kLook * m_fFocusDistance
			float newPosX = m_kFocusPos.x - m_kLook.x * m_fFocusDistance;
			float newPosY = m_kFocusPos.y - m_kLook.y * m_fFocusDistance;
			float newPosZ = m_kFocusPos.z - m_kLook.z * m_fFocusDistance;
			SetPosition(newPosX, newPosY, newPosZ);
		}
	}
}
//-----------------------------------------------------------------------
