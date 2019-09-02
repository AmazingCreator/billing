#ifndef BILLING_INI_H
#define BILLING_INI_H

#include <cstdio>
#include <set>
#include <string>
#include <cstdlib>
#include <memory.h>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <cctype>

using namespace std;
/*
 * lightweight ini reader
 */

inline void ltrim(std::basic_string<char> & s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         [](int ch) { return !std::isspace(ch); }));
}

inline void rtrim(std::basic_string<char> & s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

class Ini{
    typedef std::map<std::string, std::string> Entry;
    typedef std::map<std::string, Entry> Section;
public:
    explicit Ini(const std::string & filename);
    ~Ini()=default;
    int ReadInt(const std::string& section_key, const std::string& key, int default_value=-1);
    std::string& ReadString(const std::string& section_key, const std::string& key);
private:
    void parse(std::ifstream & is);
    Section section;
    const std::string EMPTY;
};

#endif