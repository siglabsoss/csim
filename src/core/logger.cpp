#include <syslog.h>
#include <cstdarg>
#include <core/logger.hpp>


namespace
{
constexpr size_t LOGGER_BUF_SIZE = 1024;
char logBuf[LOGGER_BUF_SIZE];
}

void Logger::init()
{
    openlog("retrosim", LOG_CONS | LOG_NDELAY, LOG_USER);
}

void Logger::debug(const char *fmt, ...)
{
    int priority = LOG_USER | LOG_DEBUG;
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(logBuf, LOGGER_BUF_SIZE, fmt, ap);
    if (size < LOGGER_BUF_SIZE - 1) {
        logBuf[size]     = '\n';
        logBuf[size + 1] = '\0';
    } else {
        logBuf[LOGGER_BUF_SIZE - 2] = '\n';
        logBuf[LOGGER_BUF_SIZE - 1] = '\0';
    }
    syslog(priority, logBuf);
    printf(logBuf);
}

void Logger::debug(std::stringstream &ss)
{
    Logger::debug(ss.str().c_str());
}

void Logger::info(const char *fmt, ...)
{
    int priority = LOG_USER | LOG_INFO;
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(logBuf, LOGGER_BUF_SIZE, fmt, ap);
    if (size < LOGGER_BUF_SIZE - 1) {
        logBuf[size]     = '\n';
        logBuf[size + 1] = '\0';
    } else {
        logBuf[LOGGER_BUF_SIZE - 2] = '\n';
        logBuf[LOGGER_BUF_SIZE - 1] = '\0';
    }
    syslog(priority, logBuf);
    printf(logBuf);
}

void Logger::info(std::stringstream &ss)
{
    Logger::info(ss.str().c_str());
}

void Logger::warn(const char *fmt, ...)
{
    int priority = LOG_USER | LOG_WARNING;
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(logBuf, LOGGER_BUF_SIZE, fmt, ap);
    if (size < LOGGER_BUF_SIZE - 1) {
        logBuf[size]     = '\n';
        logBuf[size + 1] = '\0';
    } else {
        logBuf[LOGGER_BUF_SIZE - 2] = '\n';
        logBuf[LOGGER_BUF_SIZE - 1] = '\0';
    }
    syslog(priority, logBuf);
    printf(logBuf);
}

void Logger::warn(std::stringstream &ss)
{
    Logger::warn(ss.str().c_str());
}

void Logger::error(const char *fmt, ...)
{
    int priority = LOG_USER | LOG_ERR;
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(logBuf, LOGGER_BUF_SIZE, fmt, ap);
    if (size < LOGGER_BUF_SIZE - 1) {
        logBuf[size]     = '\n';
        logBuf[size + 1] = '\0';
    } else {
        logBuf[LOGGER_BUF_SIZE - 2] = '\n';
        logBuf[LOGGER_BUF_SIZE - 1] = '\0';
    }
    syslog(priority, logBuf);
    printf(logBuf);
}

void Logger::error(std::stringstream &ss)
{
    Logger::error(ss.str().c_str());
}

void Logger::crit(const char *fmt, ...)
{
    int priority = LOG_USER | LOG_CRIT;
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(logBuf, LOGGER_BUF_SIZE, fmt, ap);
    if (size < LOGGER_BUF_SIZE - 1) {
        logBuf[size]     = '\n';
        logBuf[size + 1] = '\0';
    } else {
        logBuf[LOGGER_BUF_SIZE - 2] = '\n';
        logBuf[LOGGER_BUF_SIZE - 1] = '\0';
    }
    syslog(priority, logBuf);
    printf(logBuf);
}

void Logger::crit(std::stringstream &ss)
{
    Logger::crit(ss.str().c_str());
}
