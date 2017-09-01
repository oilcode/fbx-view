//----------------------------------------------------------------
#include "SoD3DModelBase.h"
//----------------------------------------------------------------
SoD3DModelBase::SoD3DModelBase()
:m_nModelID(-1)
,m_eModelType(SoD3DModelType_Invalid)
{

}
//----------------------------------------------------------------
SoD3DModelBase::~SoD3DModelBase()
{

}
//----------------------------------------------------------------
bool SoD3DModelBase::InitModel(void* pInitParam)
{
	return false;
}
//----------------------------------------------------------------
void SoD3DModelBase::ClearModel()
{

}
//----------------------------------------------------------------
void SoD3DModelBase::UpdateModel(float fDeltaTime)
{

}
//----------------------------------------------------------------
void SoD3DModelBase::RenderModel()
{

}
//----------------------------------------------------------------
void SoD3DModelBase::SetWorldMatrix(const XMFLOAT4X4* pMatWorld)
{

}
//----------------------------------------------------------------
