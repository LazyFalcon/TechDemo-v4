#pragma once
#include <LinearMath/btVector3.h>
#include <iostream>

namespace pmk
{
inline void toStream(std::ostream& out) {}
template<typename T>
inline void toStream(std::ostream& out, const T& val) {
    out << val << " ";
}
template<typename T, typename... Args>
inline void toStream(std::ostream& out, const T& val, Args... args) {
    toStream(out, val);
    toStream(out, args...);
}

template<>
inline void toStream(std::ostream& out, const bool& val) {
    out << (val ? "true" : "false");
}
template<>
inline void toStream(std::ostream& out, const glm::vec2& val) {
    out << "[ " << val.x << ", " << val.y << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::ivec2& val) {
    out << "[ " << val.x << ", " << val.y << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::vec4& val) {
    out << "[ " << val.x << ", " << val.y << ", " << val.z << ", " << val.w << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::ivec4& val) {
    out << "[ " << val.x << ", " << val.y << ", " << val.z << ", " << val.w << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::quat& val) {
    out << "[ " << val.x << ", " << val.y << ", " << val.z << ", " << val.w << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::vec3& val) {
    out << "[ " << val.x << ", " << val.y << ", " << val.z << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::ivec3& val) {
    out << "[ " << val.x << ", " << val.y << ", " << val.z << " ] ";
}
template<>
inline void toStream(std::ostream& out, const btVector3& val) {
    out << "[ " << val[0] << ", " << val[1] << ", " << val[2] << " ] ";
}
template<>
inline void toStream(std::ostream& out, const btQuaternion& val) {
    out << "[ " << val[0] << ", " << val[1] << ", " << val[2] << ", " << val[3] << " ] ";
}
template<>
inline void toStream(std::ostream& out, const glm::mat4& val) {
    out << "[ ";
    toStream(out, val[0]);
    toStream(out, val[1]);
    toStream(out, val[2]);
    toStream(out, val[3]);
    out << " ] ";
}
template<typename T>
inline void toStream(std::ostream& out, const std::vector<T>& val) {
    out << "{ ";
    for(auto& it : val) { toStream(out, it); }
    out << " } ";
}

extern std::vector<std::string> logBuffer;
void openLogFile();
void dumpLogBufferTofile();
std::string adjustFilename(const std::string&);
std::string adjustFuncname(const std::string&);
}

template<typename... Args>
std::string toString(const Args&... args) {
    std::stringstream ss;
    pmk::toStream(ss, args...);
    return ss.str();
}

/* TODO:
    * store output from frame, print it on the end
    * print timestamps
    * save to file, reqiures additional job to collect output
    * scope based prefixes, like in cplane
*/
enum LoggerSeverity
{
    SEVERITY_MAX,
    SEVERITY_WARNING,
    SEVERITY_INFO,
    SEVERITY_DEBUG,
    SEVERITY_ALL
};

class PmkLogger
{
private:
    std::string m_output;
    bool m_printClogs {false};
    bool m_printFile {false};
    bool m_printFunction {true};
    unsigned long int m_counter {0};
    LoggerSeverity m_severity {SEVERITY_DEBUG};

public:
    std::string prefix;

    PmkLogger();
    PmkLogger& operator()() {
        m_output = "";
        return *this;
    }

    void severity(LoggerSeverity value) {
        m_severity = value;
    }

    void printConditionalLogs() {
        m_printClogs = true;
    }

    void resetCounters() {
        m_counter = 0;
    }

    void startFrame() {
        m_printClogs = false;
        ++m_counter;
    }

    PmkLogger& regularPrefix(const std::string& filename, int line, const std::string& funcname) {
        m_output = std::string();
        if(not prefix.empty())
            m_output += prefix + " ";
        if(m_printFile)
            pmk::adjustFilename(filename) + ":" + std::to_string(line) + ": ";
        // if(m_printFunction)
        m_output += "[" + pmk::adjustFuncname(funcname) + "] ";

        return *this;
    }

    template<typename... Args>
    PmkLogger& error(const Args&... args) {
        std::cerr << "[ERROR] " << m_output << toString(args...) << "\n";
        m_output = "";

        return *this;
    }
    template<typename... Args>
    PmkLogger& warn(const Args&... args) {
        if(m_severity >= SEVERITY_WARNING) {
            std::cerr << "[WARNING] " << m_output << toString(args...) << "\n";
            m_output = "";
        }

        return *this;
    }
    template<typename... Args>
    PmkLogger& info(const Args&... args) {
        if(m_severity >= SEVERITY_INFO) {
            std::cout << m_output << toString(args...) << "\n";
            m_output = "";
        }

        return *this;
    }
    template<typename... Args>
    PmkLogger& log(const Args&... args) {
        if(m_severity >= SEVERITY_DEBUG) {
            std::cout << m_output << toString(args...) << "\n";
            m_output = "";
        }
        return *this;
    }
    template<typename... Args>
    PmkLogger& clog(const Args&... args) {
        if(m_severity >= SEVERITY_ALL or m_printClogs) {
            std::cout << m_output << toString(args...) << "\n";
            m_output = "";
        }
        return *this;
    }
    template<typename... Args>
    PmkLogger& flog(const Args&... args) {
        if(m_counter < 6 or m_printClogs) {
            std::cout << m_output << toString(args...) << "\n";
            m_output = "";
        }
        return *this;
    }
    template<typename... Args>
    PmkLogger& toFile(const Args&... args) {
        if(m_severity >= SEVERITY_ALL or m_printClogs) {
            std::cout << m_output << toString(args...) << "\n";
            m_output = "";
        }

        return *this;
    }
};

extern PmkLogger pmkLogger;

inline void hardPause() {
    std::cin.ignore();
}

class PmkLoggerPrefix
{
private:
    PmkLogger& m_logger;
    std::string m_prevoius;

public:
    PmkLoggerPrefix(PmkLogger& logger) : m_logger(logger) {}
    ~PmkLoggerPrefix() {
        m_logger.prefix = m_prevoius;
    }
    void operator()(const std::string& prefix, bool overwrite = false) {
        m_prevoius = m_logger.prefix;
        if(overwrite)
            m_logger.prefix = "[" + prefix + "]";
        else
            m_logger.prefix += "[" + prefix + "]";
    }
};

#define console pmkLogger.regularPrefix(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#define console_prefix                          \
    PmkLoggerPrefix pmkLoggerPrefix(pmkLogger); \
    pmkLoggerPrefix
