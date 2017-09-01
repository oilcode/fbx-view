//-----------------------------------------------------------------------
#ifndef _SoD3DCameraUI_h_
#define _SoD3DCameraUI_h_
//-----------------------------------------------------------------------
#include "SoD3DInclude.h"
//-----------------------------------------------------------------------
class SoD3DCameraUI
{
public:
	static bool CreateD3DCameraUI();
	static void ReleaseD3DCameraUI();
	static SoD3DCameraUI* Get();

	XMMATRIX GetMatViewProj() const;

private:
	SoD3DCameraUI();
	~SoD3DCameraUI();
	bool InitD3DCameraUI();
	void ClearD3DCameraUI();

private:
	static SoD3DCameraUI* ms_pInstance;
	//记录观察矩阵View与投影矩阵Projection相乘之后的矩阵。
	XMFLOAT4X4 m_matViewProj;
};
//-----------------------------------------------------------------------
inline SoD3DCameraUI* SoD3DCameraUI::Get()
{
	return ms_pInstance;
}
//-----------------------------------------------------------------------
#endif //_SoD3DCameraUI_h_
//-----------------------------------------------------------------------
