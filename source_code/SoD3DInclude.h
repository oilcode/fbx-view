//----------------------------------------------------------------
#ifndef _SoD3DInclude_h_
#define _SoD3DInclude_h_
//----------------------------------------------------------------
#include <D3D11.h>
#include <D3DX11tex.h>
#include <d3dx11effect.h>
#include <xnamath.h>
#include "xnacollision.h"
//----------------------------------------------------------------
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx11effect.lib")
#pragma comment(lib, "d3dcompiler.lib")
//----------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
//----------------------------------------------------------------
#include "WinApp.h"
#include "WinInputMsgManager.h"
#include "SoSimpleLog.h"
#include "SoSimpleLoadFile.h"
#include "SoArray.h"
#include "SoArrayUID.h"
#include "SoIntType.h"
#include "SoMath.h"
#include "SoStringHelp.h"
#include "SoTinyString.h"
//----------------------------------------------------------------
#include "SoD3DUserDefine.h"
//----------------------------------------------------------------
//the value for depth buffer, the max value means the far distance
#define SoD3D_DepthValue_Max 1.0f
#define SoD3D_DepthValue_Min 0.0f
//----------------------------------------------------------------
#endif //_SoD3DInclude_h_
//----------------------------------------------------------------
