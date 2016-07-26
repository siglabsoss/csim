#include <sstream>

#define log_init  Logger::init
#define log_debug Logger::debug
#define log_info  Logger::info
#define log_warn  Logger::warn
#define log_err   Logger::error
#define log_crit  Logger::crit

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
