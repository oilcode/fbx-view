//----------------------------------------------------------------
#include "GGUIWindowImage.h"
//----------------------------------------------------------------
GGUIWindowImage::GGUIWindowImage()
:m_nImagesetIndex(-1)
,m_nImageRectIndex(-1)
{
	m_eType = GGUIWindow_Image;
}
//----------------------------------------------------------------
GGUIWindowImage::~GGUIWindowImage()
{

}
//----------------------------------------------------------------
void GGUIWindowImage::SetTexture(const char* szTexture)
{
	GGUIFunc_GetImagesetIndexRectIndex(szTexture, m_nImagesetIndex, m_nImageRectIndex);
}
//----------------------------------------------------------------
void GGUIWindowImage::ClearWindow()
{
	GGUIWindowBase::ClearWindow();
	m_nImagesetIndex = -1;
	m_nImageRectIndex = -1;
}
//----------------------------------------------------------------
