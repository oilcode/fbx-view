//----------------------------------------------------------------
#ifndef _SoD3DShaderBase_h_
#define _SoD3DShaderBase_h_
//----------------------------------------------------------------
#include "SoD3DInclude.h"
//----------------------------------------------------------------
enum SoD3DShaderType
{
	SoD3DShaderType_Invalid = -1,
	SoD3DShaderType_Terrain,
	SoD3DShaderType_ModelCube,
	SoD3DShaderType_ModelFbx,
	SoD3DShaderType_ModelKK,
	SoD3DShaderType_ModelKKNoBone,
	SoD3DShaderType_GGUI,
	SoD3DShaderType_Max,
};
//----------------------------------------------------------------
class SoD3DShaderBase
{
public:
	SoD3DShaderBase();
	virtual ~SoD3DShaderBase();
	//--pParam 是一个结构体指针。不同的shader需要不同的参数，对应着不同的结构体。
	virtual void ProcessRender(void* pParam);
	void SetType(SoD3DShaderType eType);
	SoD3DShaderType GetType() const;

protected:
	SoD3DShaderType m_Type;
};
//----------------------------------------------------------------
inline void SoD3DShaderBase::SetType(SoD3DShaderType eType)
{
	m_Type = eType;
}
//----------------------------------------------------------------
inline SoD3DShaderType SoD3DShaderBase::GetType() const
{
	return m_Type;
}
//----------------------------------------------------------------
#endif //_SoD3DShaderBase_h_
//----------------------------------------------------------------
