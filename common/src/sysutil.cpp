#include "sysutil.h"
#include <direct.h>

#ifdef WIN32
#define DIV_CODE '\\'
#else
#define DIV_CODE '/'
#endif

bool SysUtil::getProcDir(std::string &path) {
    char proc[MAX_PATH];
#ifdef WIN32
    GetModuleFileName(0, proc, MAX_PATH);
    *(strrchr(proc, DIV_CODE) + 1) = 0;
#else
#endif
    path = proc;
    return true;
}

bool SysUtil::getPathDir(std::string &path) {
    for (int i = (int)path.length() - 1; i >= 0; --i) {
        char c = path[i];
        if (c == '/' || c == '\\') {
            path.resize(i);
            return true;
        }
    }
    return false;
}

bool  SysUtil::makePathDir(std::string &path) {
    if (path[0] != '/' && path[0] != '\\') {
        std::string proc;
        if (!SysUtil::getProcDir(proc)) {
            return false;
        }
        path = proc + path;
    }
    else {
        path[0] = DIV_CODE;
    }

    char *last, *token = strtok_s(&path[0], "\\/", &last);
    while (token != 0) {
#ifdef WIN32
        if (strchr(token, ':') == 0) {
            if (_mkdir(path.c_str()) != 0) {
                uint32 attr = GetFileAttributes(path.c_str());
                if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    return false;
                }
            }
        }
#else
        if (mkdir(S_IRWXU | S_IRWXG | S_IRWXO)) {
            struct stat fileStat;
            if ((stat(now_path, &fileStat) == 0) && !S_ISDIR(fileStat.st_mode)) {
                return false;
            }
        }
#endif
        if (*last != 0) {
            last[-1] = DIV_CODE;
        }
        token = strtok_s(0, "\\/", &last);
    }
    if (last[-1] == 0) {
        last[-1] = DIV_CODE;
    } else {
        path += DIV_CODE;
    }
    return true;
}
