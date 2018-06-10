#pragma once
#include <iostream>
#include <LinearMath/btVector3.h>
#include <ctime>

extern bool CLOG_SPECIAL_VALUE;

namespace pmk {
    inline void toStream(std::ostream& out){}
    template <typename T>
    inline void toStream(std::ostream& out, T val){
        out<<val<<" ";
    }
    template <typename T, typename... Args>
    inline void toStream(std::ostream& out, T val, Args... args){
        toStream(out, val);
        toStream(out, args...);
    }

    template <>
    inline void toStream(std::ostream& out, glm::vec2 val){
        out<<"[ "<<val.x<<", "<<val.y<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, glm::ivec2 val){
        out<<"[ "<<val.x<<", "<<val.y<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, glm::vec4 val){
        out<<"[ "<<val.x<<", "<<val.y<<", "<<val.z<<", "<<val.w<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, glm::ivec4 val){
        out<<"[ "<<val.x<<", "<<val.y<<", "<<val.z<<", "<<val.w<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, glm::quat val){
        out<<"[ "<<val.x<<", "<<val.y<<", "<<val.z<<", "<<val.w<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, glm::vec3 val){
        out<<"[ "<<val.x<<", "<<val.y<<", "<<val.z<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, glm::ivec3 val){
        out<<"[ "<<val.x<<", "<<val.y<<", "<<val.z<<" ] ";
    }
    template <>
    inline void toStream(std::ostream& out, btVector3 val){
        out<<"[ "<<val[0]<<", "<<val[1]<<", "<<val[2]<<" ] ";
    }
    template <typename T>
    inline void toStream(std::ostream& out, const std::vector<T> &val){
        out<<"{ ";
        for(auto &it : val){
            toStream(out, it);
        }
        out<<" } ";
    }

    extern std::vector<std::string> logBuffer;
    void openLogFile();
    void dumpLogBufferTofile();
}
// TODO: overloads for simple types (float , int, etc)
template <typename... Args>
std::string toString(const Args &... args){
    std::stringstream ss;
    pmk::toStream(ss, args...);
    return ss.str();
}

template <typename... Args>
void toFile(const Args &... args){
    std::stringstream ss;
    pmk::toStream(ss, args...);
    pmk::logBuffer.push_back(toString(ss.str()));
}

template <typename... Args>
void log(const Args &... args){
    pmk::toStream(std::cout, args...);
    std::cout<<std::endl;
}

template <typename... Args>
bool clog(const Args &... args){
    if(not CLOG_SPECIAL_VALUE) return false;
    pmk::toStream(std::cout, args...);
    std::cout<<std::endl;
    return CLOG_SPECIAL_VALUE;
}
template <typename... Args>
void error(const Args &... args){
    std::cerr << "[ ERROR ] ";
    std::stringstream ss;
    pmk::toStream(ss, args...);
    std::cerr << ss.str() << std::endl;
    pmk::logBuffer.push_back(toString(ss.str()));
}

template <typename... Args>
void errorWithLine(const std::string& filename, int linenum, const std::string& func, const Args &... args){
    std::cerr << "[ ERROR ] "<< filename << ":" << linenum<<":" << func <<" ";
    std::stringstream ss;
    pmk::toStream(ss, args...);
    std::cerr << ss.str() << std::endl;
    pmk::logBuffer.push_back(toString(ss.str()));
}

template <typename T, typename... Args>
void info(const T &t, const Args &... args){
    std::cout<<"[ "<<t<<" ] ";
    pmk::toStream(std::cout, args...);
    std::cout<<""<<std::endl;
}

template <typename T, typename... Args>
void infoWithLine(const std::string& filename, int linenum, const std::string& func, const T &t, const Args &... args){
    std::cout<<"[ "<<t<<" ] " << filename << ":" << linenum<<":" << func << " ";
    pmk::toStream(std::cout, args...);
    std::cout<<""<<std::endl;
}

#define LOG_LINE log(__FILE__, __LINE__);
#define error(...) errorWithLine(__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define info(...) infoWithLine(__FILE__, __LINE__, __func__, ##__VA_ARGS__)

inline void hardPause(){
    std::cin.ignore();
}
