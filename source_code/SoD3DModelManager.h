//----------------------------------------------------------------
#ifndef _SoD3DModelManager_h_
#define _SoD3DModelManager_h_
//----------------------------------------------------------------
#include "SoD3DModelCube.h"
#include "SoD3DModelFbx.h"
#include "SoD3DModelKK.h"
//----------------------------------------------------------------
class SoD3DModelManager
{
public:
	static bool CreateD3DModelManager();
	static void ReleaseD3DModelManager();
	static SoD3DModelManager* Get();

	SoD3DModelBase* CreateModel(int theType, void* pInitParam);
	void DeleteModel(int nModelID);
	SoD3DModelBase* GetModel(int nModelID);

private:
	SoD3DModelManager();
	~SoD3DModelManager();
	bool InitD3DModelManager();
	void ClearD3DModelManager();
	void DoDeleteModel(SoD3DModelBase* pModel);

private:
	static SoD3DModelManager* ms_pInstance;
	SoArrayUID m_kModelArray;
};
//----------------------------------------------------------------
inline SoD3DModelManager* SoD3DModelManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_SoD3DModelManager_h_
//----------------------------------------------------------------
