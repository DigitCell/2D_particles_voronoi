#include "ConfigParser.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>


ConfigParser::ConfigParser()
{

}

ConfigParser::ConfigParser(std::string filename) : sourceFile(filename)
{
    parse();
}

void ConfigParser::parse()
{
    std::ifstream file(sourceFile);
    if(!file.good())
        return;

    data.clear();

    std::string line;
    while(std::getline(file, line)){
        if(simplifyLine(line)){
            parseLine(line);
        }
    }

    file.close();
}

void ConfigParser::parseLine(std::string& line)
{
    // Check if equal sign exists
    size_t equalPos = line.find('=');
    if(equalPos == std::string::npos)
        return;

    // Get and trim the left and right terms
    std::string left = line.substr(0, equalPos);
    std::string right = line.substr(equalPos+1);

    left = rtrim(left);
    right = ltrim(right);

    // Iinvalid line if empty left and right terms
    if(left.length() == 0 || right.length() == 0)
        return;

    // Param names must not contain spaces
    /*if(left.find_first_of(WHITESPACE) != std::string::npos)
        return;*/

    // Do not reset already existing values
    if(exist(left))
        return;

    // Parse the value

    if(right == "true" || right == "false"){ // Is it a boolean ?

        set<bool>(left, right == "true" ? true : false, BOOL);

    } else if(right.front() == '"' && right.back() == '"' && right.length() >= 2){ // Is it a string ?

        set<std::string>(left, right.substr(1, right.size() - 2), STRING);

    } else if(isFloat(right)) { // Is it a float ?

        set<float>(left, std::stof(right), FLOAT);

    } else if(isInt(right)) { // Is it an int ?

        set<int>(left, std::stoi(right), INT);
    }
}

bool ConfigParser::simplifyLine(std::string& line)
{
    // Remove comments : everything that follow a '#'
    size_t commentStart = line.find('#');
    if(commentStart != std::string::npos){
        line.erase(line.begin() + commentStart, line.end());
    }
    // Trim the line
    line = ltrim(rtrim(line));
    return line.length() > 0; // ignore the line if empty
}

const std::string ConfigParser::WHITESPACE = " \n\r\t\f\v";
const std::string ConfigParser::FLOATCHARS = ".-0123456789";
const std::string ConfigParser::INTCHARS = "-0123456789";

std::string ConfigParser::ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string ConfigParser::rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

bool ConfigParser::isFloat(std::string& s)
{
    bool is = true;
    is = is && s.find_first_not_of(FLOATCHARS) == std::string::npos;
    is = is && std::count(s.begin(), s.end(), '.') == 1;

    if(s.find('-') != std::string::npos){
        if(std::count(s.begin(), s.end(), '-') == 1 && s.front() == '-'){
            is = is && true;
        }
    }

    return is;
}

bool ConfigParser::isInt(std::string& s)
{
    bool is = true;
    is = is && s.find_first_not_of(INTCHARS) == std::string::npos;

    if(s.find('-') != std::string::npos){
        if(std::count(s.begin(), s.end(), '-') == 1 && s.front() == '-'){
            is = is && true;
        }
    }

    return is;
}

template <typename T>
T ConfigParser::get(std::string name)
{
    T *ptr = static_cast<T*>(data[name].ptr);
    return *ptr;
}

template <typename T>
void ConfigParser::set(std::string name, T value, Type type)
{
    T *ptr = new T;
    *ptr = value;
    data[name] = {type, static_cast<void*>(ptr)};
}

bool ConfigParser::exist(std::string name)
{
    return data.find(name) != data.end();
}

int ConfigParser::getInt(std::string name)
{
    return get<int>(name);
}

float ConfigParser::getFloat(std::string name)
{
    return get<float>(name);
}

bool ConfigParser::getBool(std::string name)
{
    return get<bool>(name);
}

std::string ConfigParser::getString(std::string name)
{
    return get<std::string>(name);
}

void ConfigParser::printData()
{
    //std::cout << data.size() << std::endl;

    size_t maxLen = 0;
    for(auto it = data.begin(); it != data.end(); ++it){
        maxLen = std::max(maxLen, it->first.length());
    }
    maxLen++;

    for(auto it = data.begin(); it != data.end(); ++it){
        std::string key = it->first;
        Type type = it->second.type;

        switch(type){
        case BOOL:
            std::cout << "BOOL\t" << std::left << std::setw(maxLen) << key << (get<bool>(key) ? "true" : "false") << std::endl;
            break;
        case STRING:
            std::cout << "STRING\t" << std::left << std::setw(maxLen) << key << get<std::string>(key) << std::endl;
            break;
        case FLOAT:
            std::cout << "FLOAT\t" << std::left << std::setw(maxLen) << key << get<float>(key) << std::endl;
            break;
        case INT:
            std::cout << "INT\t" << std::left << std::setw(maxLen) << key << get<int>(key) << std::endl;
            break;
        default:
            break;
        }
    }

    std::cout << std::endl;
}

ConfigParser::~ConfigParser()
{

}
