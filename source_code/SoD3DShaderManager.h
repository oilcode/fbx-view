//----------------------------------------------------------------
#ifndef _SoD3DShaderManager_h_
#define _SoD3DShaderManager_h_
//----------------------------------------------------------------
#include "SoD3DShaderTerrain.h"
#include "SoD3DShaderModelCube.h"
#include "SoD3DShaderModelFbx.h"
#include "SoD3DShaderModelKK.h"
#include "SoD3DShaderModelKKNoBone.h"
#include "SoD3DShaderGGUI.h"
//----------------------------------------------------------------
class SoD3DShaderManager
{
public:
	static bool CreateD3DShaderManager();
	static void ReleaseD3DShaderManager();
	static SoD3DShaderManager* Get();

	SoD3DShaderBase* GetShader(int theType);

private:
	SoD3DShaderManager();
	~SoD3DShaderManager();
	bool InitD3DShaderManager();
	void ClearD3DShaderManager();

private:
	static SoD3DShaderManager* ms_pInstance;
	SoD3DShaderBase* m_pShaderList[SoD3DShaderType_Max];
};
//----------------------------------------------------------------
inline SoD3DShaderManager* SoD3DShaderManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_SoD3DShaderManager_h_
//----------------------------------------------------------------
