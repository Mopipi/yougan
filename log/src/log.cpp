#include <time.h>

#include "log.h"
#include "sysutil.h"

#define LOG_BUF_MAX 1024 * 1024
#define LOG_FILE_SUM 10
#define LOG_FILE_SIZE 20 * LOG_BUF_MAX

Log::Log(std::string path, std::string name)
    : m_enable(false), m_target(0), m_level(LV_INFO), m_curLen(0), m_maxWlen(LOG_BUF_MAX), m_maxOlen(LOG_BUF_MAX), m_fsize(0), m_file(0) {
    char file[MAX_PATH] = { 0 };
    if (SysUtil::makePathDir(path)) {
        m_path = path;
        m_name = name;

        // 记录
        //do {
        //    m_count++;
        //    sprintf_s(file, "%s%s_%d.log", m_path.c_str(), m_name.c_str(), m_count);
        //} while (access(file, 0) == 0);
        refile();
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
        // windows下 文本模式会以\r\n代表换行，若以文本模式打开，
        // 并使用write等函数写入换行符\n会自动在前面加\r，既实际写入是\r\n
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

        if (m_fsize > LOG_FILE_SIZE) {
            refile();
        }
        flush(m_output, curLen);
        m_fsize = ftell(m_file);
    }
}

void Log::refile() {
    close();

    char oldfile[MAX_PATH] = {0};
    char newfile[MAX_PATH] = {0};

    sprintf_s(oldfile, "%s%s_%d.log", m_path.c_str(), m_name.c_str(), LOG_FILE_SUM);
    remove(oldfile);
    for (int i = LOG_FILE_SUM; i > 1; --i) {
        sprintf_s(oldfile, "%s%s_%d.log", m_path.c_str(), m_name.c_str(), i - 1);
        sprintf_s(newfile, "%s%s_%d.log", m_path.c_str(), m_name.c_str(), i);
        rename(oldfile, newfile);
    }
    sprintf_s(oldfile, "%s%s_1.log", m_path.c_str(), m_name.c_str());
    sprintf_s(newfile, "%s%s.log", m_path.c_str(), m_name.c_str());
    rename(newfile, oldfile);

    m_file = fopen(newfile, "w+");
    fseek(m_file, 0, SEEK_END);
    m_fsize = ftell(m_file);
}

void Log::close() {
    if (m_file != 0) {
        fclose(m_file);
        m_file = 0;
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