#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <set>

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN //在包含windows.h前需要定义一个宏，去除winsock.h

#include <windows.h>
#include <process.h>
#include <ws2tcpip.h>
#include <unordered_map>
#include <io.h> /* access(), chmod() */

typedef unsigned __int8     uint8;
typedef signed __int8       sint8;
typedef unsigned __int16    uint16;
typedef signed __int16      sint16;
typedef unsigned __int32    uint32;
typedef signed __int32      sint32;
typedef unsigned __int64    uint64;
typedef signed __int64      sint64;
typedef unsigned long       ulong;

#elif LINUX
#include <unistd.h>

typedef uint8_t     uint8;
typedef __int8_t    sint8;
typedef uint16_t    uint16;
typedef __int16_t   sint16;
typedef uint32_t    uint32;
typedef __int32_t   sint32;
typedef uint64_t    uint64;
typedef __int64_t   sint64;
typedef int         SOCKET;

#define GetCurrentThreadId() syscall(SYS_gettid)
#endif

#define DISALLOW_COPY_AND_ASSIGN(ClassName)\
  ClassName(const ClassName &);\
  ClassName& operator=(const ClassName &)


#define TYPE_UPCAST(ptr, type, field) \
    ((type*)(((char*)(ptr)) - offsetof(type, field)))

#endif