//-----------------------------------------------------------------------
#ifndef _SoD3DCameraBase_h_
#define _SoD3DCameraBase_h_
//-----------------------------------------------------------------------
#include "SoD3DInclude.h"
//-----------------------------------------------------------------------
class SoD3DCameraBase
{
public:
	SoD3DCameraBase();
	~SoD3DCameraBase();

	void SetPosition(float x, float y, float z);
	void SetPitch(float fPitch);
	void SetYaw(float fYaw);
	void SetPitchYaw(float fPitch, float fYaw);
	//平移。
	//--fDeltaByRightDir 在Right方向上的偏移量
	//--fDeltaByLookDir 在Look方向上的偏移量
	void SetDeltaMove(float fDeltaByRightDir, float fDeltaByLookDir);
	void SetDeltaPitch(float deltaPitch);
	void SetDeltaYaw(float deltaYaw);
	void SetDeltaPitchYaw(float deltaPitch, float deltaYaw);

	//设置Frustum，并计算投影矩阵。
	//--fTopDownFOVAngle 上下视场的角度，弧度值。
	//--fAspect 纵横比，窗口宽除以窗口高。
	//--fNearZ 近裁剪面的距离。
	//--fFarZ 远裁剪面的距离。
	void SetFrustum(float fTopDownFOVAngle, float fAspect, float fNearZ, float fFarZ);
	//当摄像机的位置或者朝向发生变化，就要执行本函数来重新构建View矩阵。
	void UpdateViewMatrix();

	const XMFLOAT3& GetPosition() const;
	float GetPitch() const;
	float GetYaw() const;
	const XMFLOAT3& GetRight() const;
	const XMFLOAT3& GetUp() const;
	const XMFLOAT3& GetLook() const;
	XMMATRIX GetMatView() const;
	XMMATRIX GetMatProj() const;
	XMMATRIX GetMatViewProj() const;

protected:
	//计算轴向量。
	void MakeRotate();

protected:
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//下面三个成员变量就是计算世界矩阵最基本的三个元素。
	//世界坐标系内的位置坐标。
	XMFLOAT3 m_kPosition;
	//欧拉角，以弧度值表示，以X轴正方向为轴做的旋转。
	float m_fPitch;
	//欧拉角，以弧度值表示，以Z轴正方向为轴做的旋转。
	float m_fYaw;
	//记录上面三个元素是否发生了变化。
	bool m_bShouldUpdateViewMatrix;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	XMFLOAT3 m_kRight;
	XMFLOAT3 m_kUp;
	XMFLOAT3 m_kLook;
	XMFLOAT4X4 m_matView;
	XMFLOAT4X4 m_matProjection;
	XMFLOAT4X4 m_matViewProj;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
};
//-----------------------------------------------------------------------
inline const XMFLOAT3& SoD3DCameraBase::GetPosition() const
{
	return m_kPosition;
}
//-----------------------------------------------------------------------
inline float SoD3DCameraBase::GetPitch() const
{
	return m_fPitch;
}
//-----------------------------------------------------------------------
inline float SoD3DCameraBase::GetYaw() const
{
	return m_fYaw;
}
//-----------------------------------------------------------------------
inline const XMFLOAT3& SoD3DCameraBase::GetRight() const
{
	return m_kRight;
}
//-----------------------------------------------------------------------
inline const XMFLOAT3& SoD3DCameraBase::GetUp() const
{
	return m_kUp;
}
//-----------------------------------------------------------------------
inline const XMFLOAT3& SoD3DCameraBase::GetLook() const
{
	return m_kLook;
}
//-----------------------------------------------------------------------
inline XMMATRIX SoD3DCameraBase::GetMatView() const
{
	return XMLoadFloat4x4(&m_matView);
}
//-----------------------------------------------------------------------
inline XMMATRIX SoD3DCameraBase::GetMatProj() const
{
	return XMLoadFloat4x4(&m_matProjection);
}
//-----------------------------------------------------------------------
inline XMMATRIX SoD3DCameraBase::GetMatViewProj() const
{
	return XMLoadFloat4x4(&m_matViewProj);
}
//-----------------------------------------------------------------------
#endif //_SoD3DCameraBase_h_
//-----------------------------------------------------------------------
