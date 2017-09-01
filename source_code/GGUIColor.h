//------------------------------------------------------------
#ifndef _GGUIColor_h_
#define _GGUIColor_h_
//------------------------------------------------------------
class GGUIColor
{
public:
	GGUIColor() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) { }
	GGUIColor(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) { }
	void Clear()
	{
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		a = 1.0f;
	}

public:
	float r;
	float g;
	float b;
	float a;
};
//------------------------------------------------------------
extern const GGUIColor GGUIColor_Black;
extern const GGUIColor GGUIColor_White;
extern const GGUIColor GGUIColor_Red;
extern const GGUIColor GGUIColor_Green;
extern const GGUIColor GGUIColor_Blue;
//------------------------------------------------------------
#endif //_GGUIColor_h_
//------------------------------------------------------------
