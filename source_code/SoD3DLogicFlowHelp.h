//----------------------------------------------------------------
#ifndef _SoD3DLogicFlowHelp_h_
#define _SoD3DLogicFlowHelp_h_
//----------------------------------------------------------------
bool SoD3DLogicFlowHelp_Create();
void SoD3DLogicFlowHelp_Release();
void SoD3DLogicFlowHelp_Update(float fDeltaTime);
void SoD3DLogicFlowHelp_PreRender();
bool SoD3DLogicFlowHelp_BeginRender();
void SoD3DLogicFlowHelp_EndRender();
void SoD3DLogicFlowHelp_OnDropFileFbx(const char* szFileName);
void SoD3DLogicFlowHelp_OnDropFileKkb(const char* szFileName);
void SoD3DLogicFlowHelp_OnDropFileKkf(const char* szFileName);
void SoD3DLogicFlowHelp_OnDropFileImage(const char* szFileName);
void SoD3DLogicFlowHelp_ConvertFbx2KK();
//----------------------------------------------------------------
#endif //_SoD3DLogicFlowHelp_h_
//----------------------------------------------------------------
