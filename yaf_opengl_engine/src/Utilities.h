#ifndef Utilities_h__
#define Utilities_h__

#include <vector>
#include <cstring>
#include <string>

#include "YafMisc.h"

template<typename T>
std::vector<T> ConcatenateVectors(std::vector<T>& v1, std::vector<T>& v2)
{
    std::vector<T> result = v1;
    result.insert(result.end(), v2.begin(), v2.end());
    return result;
}

std::vector<float> ParseFloats(const std::string& s, int n)
{
    std::vector<float> floats;

    char* str = const_cast<char*>(s.c_str());
    char* pch;
    char* context = nullptr;

    pch = strtok_s(str, " ", &context);
    while (pch)
    {
        floats.push_back(static_cast<float>(atof(pch)));
        pch = strtok_s(nullptr, " ", &context);
    }

    if (floats.size() != n)
        throw YafParsingException("Incorrect number of arguments: " + s);

    return floats;
}

#endif // Utilities_h__
