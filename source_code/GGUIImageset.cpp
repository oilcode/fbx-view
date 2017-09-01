//----------------------------------------------------------------
#include "GGUIImageset.h"
#include "SoD3DTexture.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
GGUIImageset::GGUIImageset()
:m_pD3DTexture(0)
{

}
//----------------------------------------------------------------
GGUIImageset::~GGUIImageset()
{
	ClearImageset();
}
//----------------------------------------------------------------
bool GGUIImageset::InitImageset(int nInitRectCount)
{
	if (m_kRectArray.InitArray(sizeof(GGUIRect), nInitRectCount, 10) == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void GGUIImageset::ClearImageset()
{
	m_kRectArray.ClearArray();
	m_kName2IDMap.clear();
	if (m_pD3DTexture)
	{
		m_pD3DTexture->RemoveRefTexture();
		m_pD3DTexture = 0;
	}
}
//----------------------------------------------------------------
void GGUIImageset::AddRect(const SoTinyString& kName, const GGUIRect& kRect)
{
	if (GetRectID(kName) != -1)
	{
		SoLogError("GGUIImageset::AddRect : kName[%s] is already exist!", kName.GetValue());
		return;
	}

	int nID = m_kRectArray.FillAt(-1, &kRect);
	m_kName2IDMap.insert(std::make_pair(kName, nID));
}
//----------------------------------------------------------------
int GGUIImageset::GetRectID(const SoTinyString& kName) const
{
	mapName2ID::const_iterator it = m_kName2IDMap.find(kName);
	if (it != m_kName2IDMap.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
//----------------------------------------------------------------
const GGUIRect& GGUIImageset::GetRect(int nID) const
{
	void* pElement = m_kRectArray.GetAt(nID);
	if (pElement)
	{
		return (*((GGUIRect*)pElement));
	}
	else
	{
		return GGUIRect_Empty;
	}
}
//----------------------------------------------------------------
int GGUIImageset::GetRectCount() const
{
	return m_kRectArray.GetUsingElementCount();
}
//----------------------------------------------------------------
void* GGUIImageset::GetTexture() const
{
	if (m_pD3DTexture)
	{
		return m_pD3DTexture->GetTextureSRV();
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
float GGUIImageset::GetTextureWidth() const
{
	if (m_pD3DTexture)
	{
		return (float)m_pD3DTexture->GetTextureWidth();
	}
	else
	{
		return 0.0f;
	}
}
//----------------------------------------------------------------
float GGUIImageset::GetTextureHeight() const
{
	if (m_pD3DTexture)
	{
		return (float)m_pD3DTexture->GetTextureHeight();
	}
	else
	{
		return 0.0f;
	}
}
//----------------------------------------------------------------
void GGUIImageset::SetD3DTexture(SoD3DTexture* pD3DTexture)
{
	if (m_pD3DTexture)
	{
		m_pD3DTexture->RemoveRefTexture();
		m_pD3DTexture = 0;
	}
	m_pD3DTexture = pD3DTexture;
	if (m_pD3DTexture)
	{
		m_pD3DTexture->AddRefTexture();
	}
}
//----------------------------------------------------------------
