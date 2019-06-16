#include "core.hpp"
#include "Logger.hpp"
#include <fstream>

namespace pmk
{
std::vector<std::string> logBuffer;

void openLogFile(){
    // logBuffer.reserve(1000);
    // auto end = std::chrono::system_clock::now();
    // std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    // or: http://en.cppreference.com/w/cpp/chrono/c/strftime
    // logBuffer.emplace_back("#### "s+std::ctime(&end_time));
}

void dumpLogBufferTofile(){
    // std::fstream file("Log.md", std::fstream::out | std::fstream::trunc);
    // if(not file.is_open()) file.open("Log.md", std::fstream::out);

    // for(auto &record : logBuffer){
    //     file<<record<<"\n";
    // }
    // file.close();
}

std::string adjustFilename(const std::string& s){
    return s.substr(s.rfind("/")+1);
}
std::string adjustFuncname(const std::string& s){
    auto to = s.find("(");
    auto from = s.rfind(" ", to);
    return s.substr(from+1, to-from-1);
}

}

PmkLogger pmkLogger;
PmkLogger::PmkLogger(){
    m_printClogs = false;
    m_severity = SEVERITY_DEBUG;
    m_printFunction = false;
    m_printFile = false;
    prefix = "";
}
