#ifndef _SYS_UTIL_H_
#define _SYS_UTIL_H_

#include <string>

#include "define.h"

namespace SysUtil {
    // 获取进程目录
    bool getProcDir(std::string &path);

    // 获取路径目录
    bool getPathDir(std::string &path);

    // 创建路径目录
    bool makePathDir(std::string &path);
};

#endif
