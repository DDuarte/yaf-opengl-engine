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

template<typename T>
std::vector<T> ConcatenateVectors(std::vector<T>& v1, std::vector<T>& v2)
{
    std::vector<T> result = v1;
    result.insert(result.end(), v2.begin(), v2.end());
    return result;
}

std::vector<std::string> split_string(std::string str, const char c)
{
    std::vector<std::string> vec;
    auto i = str.find_last_of(c);
    while (i != std::string::npos)
    {
        vec.insert(vec.begin(), str.substr(i));
        //vec.push_back(str.substr(i));
        str = str.erase(i);

        i = str.find_last_of(c);
    }

    return vec;
}
