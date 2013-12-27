#ifndef Utilities_h__
#define Utilities_h__

#include <vector>
#include <cstring>
#include <string>

#include "YafMisc.h"

template<typename T>
std::vector<T> ConcatenateVectors(std::vector<T>& v1, std::vector<T>& v2);

template<unsigned int N, typename T = float>
std::vector<T> ParseNumbers(const std::string& s)
{
    std::vector<T> numbers;

    char* str = const_cast<char*>(s.c_str());
    char* pch;
    char* context = nullptr;

    pch = strtok_s(str, " ", &context);
    while (pch)
    {
        numbers.push_back(static_cast<T>(atof(pch)));
        pch = strtok_s(nullptr, " ", &context);
    }

    if (numbers.size() != N)
        throw YafParsingException("Incorrect number of arguments: " + s);

    return numbers;
}

bool starts_with(const std::string& str, const std::string& prefix); ///< Returns true if str starts with prefix
bool ends_with(const std::string& str, const std::string& suffix); ///< Returns true if str ends with suffix
bool contains(const std::string& str, const std::string fix); ///< Returns true if str contains fix
std::string to_lower(std::string str); // Converts a string to all lower case
std::vector<std::string> split_string(std::string str, const char c);

#endif // Utilities_h__
