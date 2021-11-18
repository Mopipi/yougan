#include "os.h"

uint32 GetTickTime() {
#ifdef WIN32
    return timeGetTime(); // timeGetTime() 震动频率更小精确
    //return GetTickCount(); // GetTickCount() 不精确，会因为sleep()使cpu频率停止而停止
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}