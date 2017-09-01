//----------------------------------------------------------------
#ifndef _SoMath_h_
#define _SoMath_h_
//----------------------------------------------------------------
#include <math.h>
//----------------------------------------------------------------
#define SoMath_float_zero_critical 0.001f
#define SoMath_FloatMax FLT_MAX
#define SoMath_FloatMin FLT_MIN
#define SoMath_IsFloatZero(fValue) ((-SoMath_float_zero_critical < (fValue) && (fValue) < SoMath_float_zero_critical) ? true : false)
#define SoMath_IsFloatZero2(fValue, fCritical) ((-fCritical < (fValue) && (fValue) < fCritical) ? true : false)
//----------------------------------------------------------------
#define SoMath_PI 3.14159265359f
#define SoMath_halfPI 1.57079632679f
#define SoMath_2PI 6.28318530718f
#define SoMath_1overPI 0.31830988618f
//SoMath_SinParam1 = 4 / pi
#define SoMath_SinParam1 1.27323954474f
//SoMath_SinParam2 = 4 /pi / pi
#define SoMath_SinParam2 0.40528473456f
//----------------------------------------------------------------
#define SoMath_Sin1(fRadian, fResult) {fResult = sin(fRadian);}
//SoMath_Sin2 is low precision
#define SoMath_Sin2(fRadian, fResult) {fResult=(fRadian>0.0f)?(SoMath_SinParam1*fRadian-SoMath_SinParam2*fRadian*fRadian):(SoMath_SinParam1*fRadian+SoMath_SinParam2*fRadian*fRadian);}
//SoMath_Sin3 is high precision
#define SoMath_Sin3(fRadian, fResult) {SoMath_Sin2(fRadian, fResult); fResult=(fResult>0.0f)?(0.225f*(fResult*fResult-fResult)+fResult):(0.225f*(fResult*-fResult-fResult)+fResult);}
#define SoMath_Cos1(fRadian, fResult) {fResult = cos(fRadian);}
//SoMath_Cos2 is low precision
#define SoMath_Cos2(fRadian, fResult) {float ft=fRadian; ft+=SoMath_halfPI; if(ft>SoMath_PI){ft-=SoMath_2PI;} SoMath_Sin2(ft, fResult);}
//SoMath_Cos3 is high precision
#define SoMath_Cos3(fRadian, fResult) {float ft=fRadian; ft+=SoMath_halfPI; if(ft>SoMath_PI){ft-=SoMath_2PI;} SoMath_Sin3(ft, fResult);}
//----------------------------------------------------------------
#define SoMath_Sin SoMath_Sin3
#define SoMath_Cos SoMath_Cos3
//----------------------------------------------------------------
//http://www.cnblogs.com/nsnow/archive/2010/08/09/1796111.html
//http://kb.cnblogs.com/page/189867/
//----------------------------------------------------------------
union SoMath_SqrtUnionParam
{
	int intPart;
	float floatPart;
};
#define SoMath_Sqrt1(fValue, fResult) {fResult = sqrt(fValue);}
//SoMath_Sqrt2 is only for Windows platform
#define SoMath_Sqrt2(fValue, fResult) {SoMath_SqrtUnionParam kU1, kU2; kU1.floatPart=fValue; kU2.floatPart=fValue; kU1.intPart=0x1FBCF800+(kU1.intPart>>1); kU2.intPart=0x5f375a86-(kU2.intPart>>1); fResult=0.5f*(kU1.floatPart+fValue*kU2.floatPart);} //0x5f3759df
#define SoMath_InvSqrt1(fValue, fResult) {fResult = 1.0f / sqrt(fValue);}
//SoMath_InvSqrt2 is only for Windows platform
#define SoMath_InvSqrt2(fValue, fResult) {float ft=fValue; int ni=*(int*)&ft; ni=0x5f375a86-(ni>>1); ft=*(float*)&ni; fResult=ft*(1.5f-0.5f*fValue*ft*ft);}
//----------------------------------------------------------------
#define SoMath_Sqrt SoMath_Sqrt2
#define SoMath_InvSqrt SoMath_InvSqrt2
//----------------------------------------------------------------
struct SoMathFloat2
{
	float x;
	float y;

	SoMathFloat2() : x(0.0f), y(0.0f) {};
	SoMathFloat2(float _x, float _y) : x(_x), y(_y) {};
};
extern const SoMathFloat2 SoMathFloat2_Zero;
//----------------------------------------------------------------
struct SoMathFloat3
{
	float x;
	float y;
	float z;

	SoMathFloat3() : x(0.0f), y(0.0f), z(0.0f) {};
	SoMathFloat3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
};
extern const SoMathFloat3 SoMathFloat3_Zero;
//----------------------------------------------------------------
void SoMath_VectorNormalize(SoMathFloat3* pV);
float SoMath_VectorDot(const SoMathFloat3* pV1, const SoMathFloat3* pV2);
void SoMath_VectorCross(const SoMathFloat3* pV1, const SoMathFloat3* pV2, SoMathFloat3* pResult, bool bNormalize);
//----------------------------------------------------------------
struct SoMathMatrix3
{
	float element[3][3];
};
//----------------------------------------------------------------
struct SoMathMatrix4
{
	float element[4][4];

	SoMathMatrix4();
	SoMathMatrix4(bool bIdentity);
};
extern const SoMathMatrix4 SoMathMatrix4_Identity;
//----------------------------------------------------------------
void SoMath_MatrixIdentity(SoMathMatrix4* pMat);
//----------------------------------------------------------------
//Quaternion = w + xi + yj + zk
struct SoMathQuaternion
{
	float x;
	float y;
	float z;
	float w;
};
//----------------------------------------------------------------
void SoMath_QuaternionRotate(const SoMathFloat3* pAxis, float fRadian, const SoMathFloat3* pSrcVector, SoMathFloat3* pResultVector, bool bNormalize);
//----------------------------------------------------------------
#endif //_SoMath_h_
//----------------------------------------------------------------
