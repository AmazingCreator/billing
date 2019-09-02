#include "ini.h"

Ini::Ini(const std::string &filename) {
    std::ifstream is(filename);
    if(is){
        parse(is);
        is.close();
    }
}

void Ini::parse(std::ifstream & is) {
    std::string line;
    std::string section_key;
    while (!is.eof()) {
        std::getline(is, line);
        ltrim(line);
        rtrim(line);
        const auto length = line.length();
        if (length > 0) {
            const auto pos = line.find_first_of('=');
            const auto & front = line.front();
            if (front == ';') {
                continue;
            }
            else if (front == '[') {
                if (line.back() == ']')
                    section_key = line.substr(1, length - 2);
            }
            else if (pos != 0 && pos != std::string::npos) {
                std::string variable(line.substr(0, pos));
                std::string value(line.substr(pos + 1, length));
                rtrim(variable);
                ltrim(value);
                auto & sec = section[section_key];
                if (sec.find(variable) == sec.end())
                    sec.insert(std::make_pair(variable, value));
            }
        }
    }
}

int Ini::ReadInt(const std::string& section_key, const std::string& key, int default_value) {
    auto & sec = section[section_key];
    auto value = sec.find(key);
    
    if(value == sec.end()){
        return default_value;
    }else{
        return atoi((*value).second.c_str());
    }
}

std::string &Ini::ReadString(const std::string& section_key, const std::string& key)  {
    auto & sec = section[section_key];
    auto value = sec.find(key);

    if(value == sec.end()){
        return const_cast<string &>(EMPTY);
    }else{
        return (*value).second;
    }
}
