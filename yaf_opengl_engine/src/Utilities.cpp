#include "Utilities.h"

#include <algorithm>

bool starts_with(const std::string& str, const std::string& prefix)
{
    return !str.compare(0, prefix.size(), prefix);
}

bool ends_with(const std::string& str, const std::string& suffix)
{
    return !str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

bool contains(const std::string& str, const std::string fix)
{
    return str.find(fix) != std::string::npos;
}

std::string to_lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
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

template<typename T>
std::vector<T> ConcatenateVectors(std::vector<T>& v1, std::vector<T>& v2)
{
    std::vector<T> result = v1;
    result.insert(result.end(), v2.begin(), v2.end());
    return result;
}
