#pragma once

#include <vector>
#include <iostream>
#include <sstream>

namespace Log {

enum class Level {
    Debug,
    Info,
    Warning,
    Critical
};

Log::Level global_level = Log::Level::Debug;

class Logger {
public:
    Logger(Log::Level level, std::string domain) : level(level), domain(domain) 
    {
        if (level >= global_level)
            output << __TIMESTAMP__ << " : " << domain << ": ";
    };

    ~Logger() {
        if (level >= global_level)
            std::cout << output.str() << std::endl;
    };

    template<typename T> Logger& operator<<(T const& rhs) {
        if (level >= global_level)
            output << " " << rhs;
        return *this;
    }
private:
    std::ostringstream output;
    std::string domain;
    Log::Level level;
};

};
