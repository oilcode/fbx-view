//-----------------------------------------------------------------------
#include "SoD3DCameraUI.h"
//-----------------------------------------------------------------------
SoD3DCameraUI* SoD3DCameraUI::ms_pInstance = NULL;
//-----------------------------------------------------------------------
bool SoD3DCameraUI::CreateD3DCameraUI()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new SoD3DCameraUI;
		if (ms_pInstance && ms_pInstance->InitD3DCameraUI())
		{
			br = true;
		}
		else
		{
			ReleaseD3DCameraUI();
			br = false;
		}
	}
	return br;
}
//-----------------------------------------------------------------------
void SoD3DCameraUI::ReleaseD3DCameraUI()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//-----------------------------------------------------------------------
SoD3DCameraUI::SoD3DCameraUI()
{

}
//-----------------------------------------------------------------------
SoD3DCameraUI::~SoD3DCameraUI()
{
	ClearD3DCameraUI();
}
//-----------------------------------------------------------------------
bool SoD3DCameraUI::InitD3DCameraUI()
{
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//GenerateMatrixView
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Look = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR Pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX matView = XMMatrixLookToRH(Pos, Look, Up);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//GenerateMatrixProjection
	const float width = (float)(WinApp::Get()->GetClientWidth());
	const float height = (float)(WinApp::Get()->GetClientHeight());
	const float fNearZ = 0.0f;
	const float fFarZ = 10000.0f;
	XMMATRIX matProj = XMMatrixOrthographicOffCenterRH(0.0f, width, height, 0.0f, fNearZ, fFarZ);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	XMMATRIX Result = XMMatrixMultiply(matView, matProj);
	XMStoreFloat4x4(&m_matViewProj, Result);
	return true;
}
//-----------------------------------------------------------------------
void SoD3DCameraUI::ClearD3DCameraUI()
{
	//do nothing
}
//-----------------------------------------------------------------------
XMMATRIX SoD3DCameraUI::GetMatViewProj() const
{
	return XMLoadFloat4x4(&m_matViewProj);
}
//-----------------------------------------------------------------------
