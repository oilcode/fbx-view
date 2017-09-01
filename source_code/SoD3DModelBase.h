//----------------------------------------------------------------
#ifndef _SoD3DModelBase_h_
#define _SoD3DModelBase_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
//----------------------------------------------------------------
enum SoD3DModelType
{
	SoD3DModelType_Invalid = -1,
	SoD3DModelType_Cube,
	SoD3DModelType_Max,
};
//----------------------------------------------------------------
class SoD3DModelBase
{
public:
	SoD3DModelBase();
	virtual ~SoD3DModelBase();

	virtual bool InitModel(void* pInitParam);
	virtual void ClearModel();
	virtual void UpdateModel(float fDeltaTime);
	virtual void RenderModel();
	virtual void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);

	void SetModelID(int nID);
	int GetModelID() const;
	void SetModelType(SoD3DModelType eType);
	SoD3DModelType GetModelType() const;

protected:
	int m_nModelID;
	SoD3DModelType m_eModelType;
};
//----------------------------------------------------------------
inline void SoD3DModelBase::SetModelID(int nID)
{
	m_nModelID = nID;
}
//----------------------------------------------------------------
inline int SoD3DModelBase::GetModelID() const
{
	return m_nModelID;
}
//----------------------------------------------------------------
inline void SoD3DModelBase::SetModelType(SoD3DModelType eType)
{
	m_eModelType = eType;
}
//----------------------------------------------------------------
inline SoD3DModelType SoD3DModelBase::GetModelType() const
{
	return m_eModelType;
}
//----------------------------------------------------------------
#endif //_SoD3DModelBase_h_
//----------------------------------------------------------------
