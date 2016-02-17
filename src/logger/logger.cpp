#include <string>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <map>
#include "logger.h"

using std::map;
using std::ostream;
using std::endl;
using std::function;
using std::string;
using std::stringstream;

logmsg::logmsg(level s, string m, string f, int l, string fn)
{
    msg=m;
    file=f;
    line=l;
    severity=s;
    time = std::chrono::system_clock::now();
    func = fn;
}

logger::logger(ostream& os)
    : _out(os)
{
    _levels[level::fatal] = "F!";
    _levels[level::error] = "E!";
    _levels[level::warning] = "W";
    _levels[level::info] = "I";
    _levels[level::debug] = "D";
    _levels[level::trace] = "T";

    _formatter = [this](const logmsg& m,bool divider){
	return default_formatter(m,divider);
    };

    bootstrap();
}

logger::~logger()
{
    endsession();
}

string logger::default_formatter(const logmsg& m, bool divider)
{
    char buffer[128];
    stringstream s;
    std::time_t tm = std::chrono::system_clock::to_time_t(m.time);
    struct tm* timeinfo;
    timeinfo=localtime(&tm);
    strftime(buffer,128,"%D %R:%S",timeinfo);
    s << std::setw(17) << std::left << buffer << ":" 
      << std::setw(2) << _levels[m.severity] << ": ";

    if (divider)
	s << std::setw(65) << std::setfill('-');

    s  << m.msg << std::endl;
    return s.str();
}

bool logger::log(const logmsg& m, bool divider)
{
    #ifndef DEBUG
    if ( (m.severity==level::info) || (m.severity==level::debug) )
	return false;
    #endif

    if ( enabled(m.severity))
	_out << _formatter(m,divider);
    return true;
}

bool logger::log_leave(const logmsg& m, bool divider)
{
    logmsg mm(m);
    mm.msg = "Leaving " + m.func;
    return log(mm,divider);
}

bool logger::log(const logmsg& m, const map<string,string>& params, bool divider)
{
    logmsg m_params(m);
    stringstream s;
    s << m.msg << " - params: ";
    for ( auto param : params)
    {
	s << param.first << "=" << param.second << " ";
    }
    m_params.msg = s.str();
    return log(m_params,divider);
}

void logger::set_formatter( function<log_format_t> f)
{
    _formatter = f;
}

bool logger::bootstrap()
{
    return true;
}

bool logger::endsession()
{
    return true;
}

void logger::enable(level l)
{
    _enabled_levels.insert(l);
}

void logger::disable(level l)
{
    _enabled_levels.erase(l);
}

bool logger::enabled(level l)
{
    return _enabled_levels.find(l)!=_enabled_levels.end();
}
