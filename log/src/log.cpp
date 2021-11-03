#include <time.h>

#include "log.h"
#include "sysutil.h"

#define LOG_MAX 1024 * 1024

Log::Log(std::string path, std::string name)
    : m_enable(false), m_target(0), m_level(LV_INFO), m_curLen(0), m_maxWlen(LOG_MAX), m_maxOlen(LOG_MAX), m_count(0), m_file(0) {
    if (SysUtil::makePathDir(path)) {
        char file[MAX_PATH] = {0};
        sprintf_s(file, "%s%s.log", path.c_str(), name.c_str());
        m_file = fopen(file, "w+");
    }
    m_write = new char[m_maxWlen];
    m_output = new char[m_maxOlen];
}

Log::~Log() {

}

void Log::start() {
    m_enable = true;
}

void Log::setTarget(uint32 target) {
    m_target = target;
}

void Log::write(uint32 level, const char* fmt, ...) {
    if (level < m_level)
        return;

    m_lock.lock();

    // todo: 超出预设的大小 要重新resize
    //if (m_curLen + len >= LOG_MAX) {

    //}

    // todo: 时间头根据配置需求
    int len = 0;
    if (true) {
        time_t t = time(0);
        tm *pTm = localtime(&t);
        len = strftime(m_write + m_curLen, m_maxWlen, "[%H:%M:%S] ", pTm);
    }
    va_list args;
    va_start(args, fmt);
    len += ::vsnprintf(m_write + m_curLen + len, m_maxWlen, fmt, args);
    va_end(args);

    if (m_enable) {
        m_curLen += len;
        m_write[m_curLen++] = '\n';
        m_write[m_curLen] = 0;

    }
    else {
        m_write[len++] = '\n';
        m_write[len] = 0;
        flush(m_write, len);
    }
    m_lock.unlock();
}

void Log::output() {
    if (m_curLen != 0) {
        m_lock.lock();
        uint32 swapMaxLen = m_maxOlen;
        m_maxOlen = m_maxWlen;
        m_maxWlen = swapMaxLen;

        char *swapBuf = m_output;
        m_output = m_write;
        m_write = swapBuf;

        uint32 curLen = m_curLen;
        m_curLen = 0;
        m_lock.unlock();

        flush(m_output, curLen);
    }
}

void Log::flush(const char *str, uint32 len) {
    if (m_target | LOG_STDOUT) {
        printf("%s", str);
        fflush(stdout);
    }

    if (m_target | LOG_FILE) {
        fwrite(str, sizeof(char), len, m_file);
        fflush(m_file);
    }
}