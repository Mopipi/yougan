#ifndef _LOG_H_
#define _LOG_H_

#include "synclock.h"

enum {
    LV_NONE, //
    LV_DEBUG,
    LV_INFO,
    LV_WARN,
    LV_ERROR,
    LV_FATAL,
};

enum {
    FILE_REOPEN, //新开文件
    FILE_APPEND,// 追加文件
};

#define LOG_STDOUT  0x01 // 终端
#define LOG_FILE    0x02 // 文件

class Log {
    DISALLOW_COPY_AND_ASSIGN(Log);
public:
    Log(std::string path, std::string name, int mode);
    ~Log();
public:
    void start();
    void setTarget(uint32 target);
    void write(uint32 level, const char* fmt, ...);
private:
    friend class LogModule;
    void output();
    void fopen();
    void refile();
    void close();
    void flush(const char *str, uint32 len);
private:
    bool m_enable;
    uint32 m_target;
    uint32 m_level;
    SpinLock m_lock;

    uint32 m_curLen;
    uint32 m_maxWlen;
    char *m_write;
    uint32 m_maxOlen;
    char *m_output;

    std::string m_path;
    std::string m_name;

    uint32 m_mode;
    uint32 m_count;
    uint32 m_fsize;
    FILE *m_file;
};

#endif