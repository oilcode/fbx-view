//-----------------------------------------------------------------------
//1，摄像机的世界矩阵，最基本的输入是m_kPosition，m_fPitch，m_fYaw三个元素，
//   根据这三个元素可以计算得到三个轴向量，然后就可以得到摄像机矩阵。
//-----------------------------------------------------------------------
#include "SoD3DCameraBase.h"
//-----------------------------------------------------------------------
SoD3DCameraBase::SoD3DCameraBase()
:m_kPosition(0.0f, 0.0f, 0.0f)
,m_fPitch(0.0f)
,m_fYaw(0.0f)
,m_bShouldUpdateViewMatrix(true)
{

}
//-----------------------------------------------------------------------
SoD3DCameraBase::~SoD3DCameraBase()
{

}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetPosition(float x, float y, float z)
{
	m_kPosition.x = x;
	m_kPosition.y = y;
	m_kPosition.z = z;
	m_bShouldUpdateViewMatrix = true;
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetPitch(float fPitch)
{
	SetDeltaPitch(fPitch - m_fPitch);
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetYaw(float fYaw)
{
	SetDeltaYaw(fYaw - m_fYaw);
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetPitchYaw(float fPitch, float fYaw)
{
	SetDeltaPitchYaw(fPitch - m_fPitch, fYaw - m_fYaw);
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetDeltaMove(float fDeltaByRightDir, float fDeltaByLookDir)
{
	if (SoMath_IsFloatZero(fDeltaByRightDir) == false)
	{
		//m_kPosition += m_kRight * fDeltaByRightDir
		XMVECTOR s = XMVectorReplicate(fDeltaByRightDir);
		XMVECTOR r = XMLoadFloat3(&m_kRight);
		XMVECTOR p = XMLoadFloat3(&m_kPosition);
		XMStoreFloat3(&m_kPosition, XMVectorMultiplyAdd(s, r, p));
		m_bShouldUpdateViewMatrix = true;
	}
	if (SoMath_IsFloatZero(fDeltaByLookDir) == false)
	{
		//m_kPosition += m_kLook * fDeltaByLookDir
		XMVECTOR s = XMVectorReplicate(fDeltaByLookDir);
		XMVECTOR l = XMLoadFloat3(&m_kLook);
		XMVECTOR p = XMLoadFloat3(&m_kPosition);
		XMStoreFloat3(&m_kPosition, XMVectorMultiplyAdd(s, l, p));
		m_bShouldUpdateViewMatrix = true;
	}
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetDeltaPitch(float deltaPitch)
{
	if (SoMath_IsFloatZero(deltaPitch) == false)
	{
		m_fPitch += deltaPitch;
		MakeRotate();
	}
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetDeltaYaw(float deltaYaw)
{
	if (SoMath_IsFloatZero(deltaYaw) == false)
	{
		m_fYaw += deltaYaw;
		MakeRotate();
	}
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetDeltaPitchYaw(float deltaPitch, float deltaYaw)
{
	if (SoMath_IsFloatZero(deltaPitch) == false || SoMath_IsFloatZero(deltaYaw) == false)
	{
		m_fPitch += deltaPitch;
		m_fYaw += deltaYaw;
		MakeRotate();
	}
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::SetFrustum(float fTopDownFOVAngle, float fAspect, float fNearZ, float fFarZ)
{
	XMMATRIX P = XMMatrixPerspectiveFovRH(fTopDownFOVAngle, fAspect, fNearZ, fFarZ);
	XMStoreFloat4x4(&m_matProjection, P);
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::UpdateViewMatrix()
{
	if (m_bShouldUpdateViewMatrix)
	{
		m_bShouldUpdateViewMatrix = false;
		//
		XMVECTOR U = XMLoadFloat3(&m_kUp);
		XMVECTOR L = XMLoadFloat3(&m_kLook);
		XMVECTOR P = XMLoadFloat3(&m_kPosition);

		XMMATRIX matView = XMMatrixLookToRH(P, L, U);
		XMStoreFloat4x4(&m_matView, matView);

		XMMATRIX matProj = XMLoadFloat4x4(&m_matProjection);
		XMStoreFloat4x4(&m_matViewProj, XMMatrixMultiply(matView, matProj));
	}
}
//-----------------------------------------------------------------------
void SoD3DCameraBase::MakeRotate()
{
	XMVECTOR vecRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vecUp = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMMATRIX matRotateRightDir = XMMatrixRotationAxis(vecRight, m_fPitch);
	XMMATRIX matRotateUpDir = XMMatrixRotationAxis(vecUp, m_fYaw);
	//
	vecRight = XMVector3TransformNormal(vecRight, matRotateUpDir);
	vecUp = XMVector3TransformNormal(vecUp, matRotateRightDir);
	vecUp = XMVector3TransformNormal(vecUp, matRotateUpDir);
	XMVECTOR vecLook = XMVector3Cross(vecUp, vecRight);
	//
	XMStoreFloat3(&m_kRight, vecRight);
	XMStoreFloat3(&m_kUp, vecUp);
	XMStoreFloat3(&m_kLook, vecLook);
	m_bShouldUpdateViewMatrix = true;
}
//-----------------------------------------------------------------------
