//--------------------------------------------------------------------
#ifndef _SoIntType_h_
#define _SoIntType_h_
//--------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)
typedef __int8  soint8;
typedef __int16 soint16;
typedef __int32 soint32;
typedef __int32 soint;
typedef __int64 soint64;
typedef unsigned __int8  souint8;
typedef unsigned __int16 souint16;
typedef unsigned __int32 souint32;
typedef unsigned __int32 souint;
typedef unsigned __int64 souint64;
#else
typedef int8_t  soint8;
typedef int16_t soint16;
typedef int32_t soint32;
typedef int32_t soint;
typedef int64_t soint64;
typedef uint8_t  souint8;
typedef uint16_t souint16;
typedef uint32_t souint32;
typedef uint32_t souint;
typedef uint64_t souint64;
#endif
//--------------------------------------------------------------------
#endif //_SoIntType_h_
//--------------------------------------------------------------------
