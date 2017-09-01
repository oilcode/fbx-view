//-----------------------------------------------------------------------
#ifndef _SoD3DCamera_h_
#define _SoD3DCamera_h_
//-----------------------------------------------------------------------
#include "SoD3DCameraBase.h"
//-----------------------------------------------------------------------
struct stInputEvent;
//-----------------------------------------------------------------------
class SoD3DCamera : public SoD3DCameraBase
{
public:
	enum eCameraMode
	{
		CameraMode_Free, //自由摄像机
		CameraMode_Focus, //聚焦摄像机，例如聚焦到客户端主角身上
	};

public:
	static bool CreateD3DCamera();
	static void ReleaseD3DCamera();
	static SoD3DCamera* Get();

	void SetCameraMode(eCameraMode newMode);
	eCameraMode GetCameraMode() const;

	void ProcessInputEvent(stInputEvent* pInputEvent);
	void UpdateFreeMove(float fDeltaTime);

	void SetFocusPos(float x, float y, float z);
	void SetFocusDistance(float fDistance);
	void SetFocusDeltaDistance(float deltaDistance);

private:
	SoD3DCamera();
	~SoD3DCamera();
	bool InitD3DCamera();
	void ClearD3DCamera();

private:
	static SoD3DCamera* ms_pInstance;
	eCameraMode m_CameraMode;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//控制摄像机旋转的成员。
	//从鼠标右键按下开始，鼠标的移动控制镜头的旋转。
	int m_nMouseRightKeyDown;
	int m_nLastCursorPosX;
	int m_nLastCursorPosY;
	//鼠标位置。以窗口左上角为原点。
	int m_nLastMousePosX;
	int m_nLastMousePosY;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//聚焦模式所需要的成员。
	//聚焦到的目标位置。
	XMFLOAT3 m_kFocusPos;
	//摄像机与目标位置的距离。
	float m_fFocusDistance;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
};
//-----------------------------------------------------------------------
inline SoD3DCamera* SoD3DCamera::Get()
{
	return ms_pInstance;
}
//-----------------------------------------------------------------------
inline void SoD3DCamera::SetCameraMode(SoD3DCamera::eCameraMode newMode)
{
	m_CameraMode = newMode;
}
//-----------------------------------------------------------------------
inline SoD3DCamera::eCameraMode SoD3DCamera::GetCameraMode() const
{
	return m_CameraMode;
}
//-----------------------------------------------------------------------
#endif //_SoD3DCamera_h_
//-----------------------------------------------------------------------
