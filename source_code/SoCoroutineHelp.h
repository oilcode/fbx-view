//------------------------------------------------------------
#ifndef _SoCoroutineHelp_h_
#define _SoCoroutineHelp_h_
//------------------------------------------------------------
#include "SoCoroutine.h"
//------------------------------------------------------------
enum SoCoroutineObjType
{
	SoCoroutineObjType_None, //SoCoroutine::m_pObjFunc的值为SoCoroutineFuncPointer。
	SoCoroutineObjType_Test, //SoCoroutine::m_pObjFunc的值为TestClass类对象的指针。
};
//------------------------------------------------------------
void SoCoroutineHelp_CallFunc(SoCoroutine* pCo);
//------------------------------------------------------------
#endif //_SoCoroutineHelp_h_
//------------------------------------------------------------
