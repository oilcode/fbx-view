//------------------------------------------------------------
#include "SoCoroutineHelp.h"
//------------------------------------------------------------
void SoCoroutineHelp_CallFunc(SoCoroutine* pCo)
{
	switch (pCo->m_nObjType)
	{
	case SoCoroutineObjType_None:
		{
			SoCoroutineFuncPointer realFunc = (SoCoroutineFuncPointer)pCo->m_pObjFunc;
			realFunc(pCo);
		} break;
	case SoCoroutineObjType_Test:
		{
			//TestClass* pTestClass = (TestClass*)pCo->m_pObjFunc;
			//pTestClass->ProcessCo(this);
		} break;
	default:
		break;
	}
}
//------------------------------------------------------------
