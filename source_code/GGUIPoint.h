//------------------------------------------------------------
#ifndef _GGUIPoint_h_
#define _GGUIPoint_h_
//------------------------------------------------------------
class GGUIPoint
{
public:
	GGUIPoint() : x(0.0f), y(0.0f) { }
	GGUIPoint(float fx, float fy) : x(fx), y(fy) { }
	void Clear()
	{
		x = 0.0f;
		y = 0.0f;
	}

public:
	float x;
	float y;
};
//------------------------------------------------------------
extern const GGUIPoint GGUIPoint_Empty;
//------------------------------------------------------------
#endif //_GGUIPoint_h_
//------------------------------------------------------------
