#ifndef LOGGER_H
#define LOGGER_H

#include <functional>
#include <iostream>
#include <string>
#include <chrono>
#include <map>

#define wlog(lev,msg) g_logger.log( logmsg(lev,msg,__FILE__,__LINE__,__PRETTY_FUNCTION__) )
#define wlog_entry()  g_logger.log( logmsg(level::debug,__PRETTY_FUNCTION__,__FILE__,__LINE__,__PRETTY_FUNCTION__) )

enum class level
{
    fatal,
	error,
	warning,
	info,
	debug
	};

class logmsg
{
public:
    logmsg(level,std::string,std::string,int,std::string);

    std::string msg;
    std::string file;
    int line;
    level severity;
    std::chrono::time_point<std::chrono::system_clock> time;
    std::string func;
};

typedef std::string log_format_t(const logmsg& );

class logger
{

 public:

    logger(std::ostream& os);
    virtual ~logger();


    virtual bool log(const logmsg&);

 protected:
    virtual bool bootstrap();
    virtual bool endsession();
    virtual void set_formatter( std::function<log_format_t> f);
    virtual std::string default_formatter(const logmsg&);

    std::function<log_format_t> _formatter;

 private:
    std::ostream& _out;
    std::map<level,std::string> _levels;
};

#endif

extern logger g_logger;
