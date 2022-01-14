#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <map>

// Parser for the custom configuration file format

class ConfigParser
{
    public:
        ConfigParser();
        ConfigParser(std::string filename);

        void parse();

        int getInt(std::string name);
        float getFloat(std::string name);
        bool getBool(std::string name);
        std::string getString(std::string name);

        bool exist(std::string name);

        void printData();

        virtual ~ConfigParser();

    protected:

    private:
        enum Type
        {
            FLOAT,
            INT,
            STRING,
            BOOL
        };

        struct Value
        {
            Type type;
            void* ptr;
        };

        std::map<std::string, Value> data;

        std::string sourceFile;

        static const std::string WHITESPACE;
        static const std::string FLOATCHARS;
        static const std::string INTCHARS;

        template <typename T>
        void set(std::string name, T value, Type type);

        template <typename T>
        T get(std::string name);

        static std::string ltrim(const std::string& s);
        static std::string rtrim(const std::string& s);
        static bool simplifyLine(std::string& line);
        static bool isFloat(std::string& s);
        static bool isInt(std::string& s);

        void parseLine(std::string& line);
};

#endif // CONFIGPARSER_H
