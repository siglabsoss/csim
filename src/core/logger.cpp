#include <syslog.h>
#include <cstdarg>
#include <core/logger.hpp>

void Logger::init()
{
    openlog("retrosim", LOG_CONS | LOG_NDELAY, LOG_USER);
}

void Logger::debug(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int priority = LOG_USER | LOG_DEBUG;
    vsyslog(priority, fmt, ap);
}

void Logger::debug(std::stringstream &ss)
{
    Logger::debug(ss.str().c_str());
}

void Logger::info(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int priority = LOG_USER | LOG_INFO;
    vsyslog(priority, fmt, ap);
}

void Logger::info(std::stringstream &ss)
{
    Logger::info(ss.str().c_str());
}

void Logger::warn(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int priority = LOG_USER | LOG_WARNING;
    vsyslog(priority, fmt, ap);
}

void Logger::warn(std::stringstream &ss)
{
    Logger::warn(ss.str().c_str());
}

void Logger::error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int priority = LOG_USER | LOG_ERR;
    vsyslog(priority, fmt, ap);
}

void Logger::error(std::stringstream &ss)
{
    Logger::error(ss.str().c_str());
}

void Logger::crit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int priority = LOG_USER | LOG_CRIT;
    vsyslog(priority, fmt, ap);
}

void Logger::crit(std::stringstream &ss)
{
    Logger::crit(ss.str().c_str());
}
