//----------------------------------------------------------------
#ifndef _GGUIWindowImage_h_
#define _GGUIWindowImage_h_
//----------------------------------------------------------------
#include "GGUIWindowBase.h"
//----------------------------------------------------------------
class GGUIWindowImage : public GGUIWindowBase
{
public:
	//--szTexture "ImagesetName:RectName"
	void SetTexture(const char* szTexture);
	int GetImagesetIndex() const;
	int GetImageRectIndex() const;

protected:
	friend class GGUIWindowFactory;
	GGUIWindowImage();
	~GGUIWindowImage();
	//re-write parent function
	void ClearWindow();

private:
	int m_nImagesetIndex;
	int m_nImageRectIndex;
};
//----------------------------------------------------------------
inline int GGUIWindowImage::GetImagesetIndex() const
{
	return m_nImagesetIndex;
}
//----------------------------------------------------------------
inline int GGUIWindowImage::GetImageRectIndex() const
{
	return m_nImageRectIndex;
}
//----------------------------------------------------------------
#endif //_GGUIWindowImage_h_
//----------------------------------------------------------------
