#include "logger.h"

#include <chrono>
#include <ctime>
using std::string;
using std::chrono::system_clock;

Logger::Logger() {
    fs.open("log.log", std::ios_base::app);
}

Logger::~Logger() {
    if (fs.is_open()) {
        fs.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::write(const char* str) {
    getInstance().doWrite(str);
}

void Logger::write(const string& str) {
    getInstance().doWrite(str);
}

void Logger::doWrite(const char* str) {
    outputCurrentTime(std::cout);
    std::cout << str << std::endl;
    outputCurrentTime(this->fs);
    this->fs << str << std::endl;
}

void Logger::doWrite(const string& str) {
    outputCurrentTime(std::cout);
    std::cout << str << std::endl;
    outputCurrentTime(this->fs);
    this->fs << str << std::endl;
}

void Logger::outputCurrentTime(std::ostream & out)
{
    auto n = system_clock::now();
    auto t = system_clock::to_time_t(n);
    char timeStr[22];;
    std::strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S]", std::localtime(&t));
    out << timeStr;
}