//--------------------------------------------------------------------
#ifndef _SoCodeBaseInclude_h_
#define _SoCodeBaseInclude_h_
//--------------------------------------------------------------------
#define SoPlatform_Windows  1
#define SoPlatform_Android  2
#define SoAppBit_32  1
#define SoAppBit_64  2
//--------------------------------------------------------------------
#if defined(WIN32) || defined(WIN64)
#define SoTargetPlatform  SoPlatform_Windows
#else
#define SoTargetPlatform  SoPlatform_Android
#endif
//--------------------------------------------------------------------
#define SoTargetAppBit    SoAppBit_32
//--------------------------------------------------------------------
#define SoMalloc  malloc
#define SoFree    free
#define SoNew     new
#define SoDelete  delete
//--------------------------------------------------------------------
#define SODLL
//--------------------------------------------------------------------
#include <cstdlib> //malloc free new delete
#include <cstdio>  //FILE
#include <cstdarg> //va_start va_start
#include <string>  //if not include, new operation may crash
//--------------------------------------------------------------------
#if (SoTargetPlatform == SoPlatform_Windows)
typedef char    soint8;
typedef __int16 soint16;
typedef __int32 soint32;
typedef __int32 soint;
typedef __int64 soint64;
typedef unsigned __int8  souint8;
typedef unsigned __int16 souint16;
typedef unsigned __int32 souint32;
typedef unsigned __int32 souint;
typedef unsigned __int64 souint64;
typedef float sofloat;
typedef double sodouble;
#else
typedef char    soint8;
typedef int16_t soint16;
typedef int32_t soint32;
typedef int32_t soint;
typedef int64_t soint64;
typedef uint8_t  souint8;
typedef uint16_t souint16;
typedef uint32_t souint32;
typedef uint32_t souint;
typedef uint64_t souint64;
typedef float sofloat;
typedef double sodouble;
#endif
//--------------------------------------------------------------------
//定义了这个宏，表示在SoLua模块内遇到错误就会弹框报错。
//在GGShipping版本下不会弹框报错。
#define SoLuaHandleError

//--------------------------------------------------------------------
#if (SoTargetPlatform == SoPlatform_Windows)

//定义了这个宏，表示启用SoStringConvert内的帮助函数；
//没有定义这个宏，表示不启用SoStringConvert内的帮助函数。
//如果启用的话，SoStringConvert会占用一点内存。
//#define SoStringConvertEnable

//定义了这个宏，表示启用SoThreadSafeResource内的帮助函数；
//没有定义这个宏，表示不启用SoThreadSafeResource内的帮助函数。
//#define SoThreadSafeResourceEnable

#elif (SoTargetPlatform == SoPlatform_Android)

//定义了这个宏，表示启用SoStringConvert内的帮助函数；
//没有定义这个宏，表示不启用SoStringConvert内的帮助函数。
//如果启用的话，SoStringConvert会占用一点内存。
//#define SoStringConvertEnable

//定义了这个宏，表示启用SoThreadSafeResource内的帮助函数；
//没有定义这个宏，表示不启用SoThreadSafeResource内的帮助函数。
//#define SoThreadSafeResourceEnable

#endif
//--------------------------------------------------------------------
typedef void* SoArrayElementTypePointer;
//--------------------------------------------------------------------
#endif //_SoCodeBaseInclude_h_
//--------------------------------------------------------------------
