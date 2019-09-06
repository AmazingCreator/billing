#include "logger.h"

#include <time.h>
using std::string;

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
    time_t t;
    time (&t);
    char timeStr[22];;
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S]", localtime(&t));
    out << timeStr;
}