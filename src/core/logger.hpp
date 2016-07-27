#include <sstream>

#define log_init  Logger::init

#ifdef NDEBUG
#  define RETROSIM_LOG_LEVEL 4
#else
#  define RETROSIM_LOG_LEVEL 5
#endif

#if RETROSIM_LOG_LEVEL > 4
#  define log_debug Logger::debug
#else
#  define log_debug
#endif

#if RETROSIM_LOG_LEVEL > 3
#  define log_info  Logger::info
#else
#  define log_info
#endif

#if RETROSIM_LOG_LEVEL > 2
#  define log_warn  Logger::warn
#else
#  define log_warn
#endif

#if RETROSIM_LOG_LEVEL > 1
#  define log_err   Logger::error
#else
#  define log_err
#endif

#if RETROSIM_LOG_LEVEL > 0
#  define log_crit  Logger::crit
#else
#  define log_crit
#endif

namespace Logger
{

void init();

void debug(const char *fmt, ...);
void debug(std::stringstream &ss);

void info(const char *fmt, ...);
void info(std::stringstream &ss);

void warn(const char *fmt, ...);
void warn(std::stringstream &ss);

void error(const char *fmt, ...);
void error(std::stringstream &ss);

void crit(const char *fmt, ...);
void crit(std::stringstream &ss);

} //end namespace Logger
