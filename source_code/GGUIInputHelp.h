//----------------------------------------------------------------
#ifndef _GGUIInputHelp_h_
#define _GGUIInputHelp_h_
//----------------------------------------------------------------
struct stInputEvent;
class GGUIWindowContainer;
class GGUIWindowText;
class GGUIWindowImage;
class GGUIWindowButton;
//----------------------------------------------------------------
void GGUIInputProcess(stInputEvent* pInputEvent);
void GGUIInput_UIContainer(const GGUIWindowContainer* pUIContainer, stInputEvent* pInputEvent);
void GGUIInput_UIText(GGUIWindowText* pUIText, stInputEvent* pInputEvent);
void GGUIInput_UIImage(GGUIWindowImage* pUIImage, stInputEvent* pInputEvent);
void GGUIInput_UIButton(GGUIWindowButton* pUIButton, stInputEvent* pInputEvent);
//----------------------------------------------------------------
#endif //_GGUIInputHelp_h_
//----------------------------------------------------------------
