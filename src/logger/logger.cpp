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

    _formatter = [this](const logmsg& m){
	return default_formatter(m);
    };

    bootstrap();
}

logger::~logger()
{
    endsession();
}

string logger::default_formatter(const logmsg& m)
{
    char buffer[128];
    stringstream s;
    std::time_t tm = std::chrono::system_clock::to_time_t(m.time);
    struct tm* timeinfo;
    timeinfo=localtime(&tm);
    strftime(buffer,128,"%D %R:%S",timeinfo);
    s << std::setw(17) << std::left << buffer << ":" 
      << std::setw(2) << _levels[m.severity] << ": "
      << m.msg << std::endl;
    return s.str();
}

bool logger::log(const logmsg& m)
{
    _out << _formatter(m);
    return true;
}
bool logger::log(const logmsg& m, const map<string,string>& params)
{
    logmsg m_params(m);
    stringstream s;
    s << m.msg << " - params: ";
    for ( auto param : params)
    {
	s << param.first << "=" << param.second << " ";
    }
    m_params.msg = s.str();
    log(m_params);
    return true;
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
