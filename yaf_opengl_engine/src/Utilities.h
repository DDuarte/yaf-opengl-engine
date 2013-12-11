#ifndef Utilities_h__
#define Utilities_h__

#include <vector>
#include <cstring>
#include <string>

#include "YafMisc.h"

template<typename T>
std::vector<T> ConcatenateVectors(std::vector<T>& v1, std::vector<T>& v2);

std::vector<float> ParseFloats(const std::string& s, int n);

bool starts_with(const std::string& str, const std::string& prefix); ///< Returns true if str starts with prefix
bool ends_with(const std::string& str, const std::string& suffix); ///< Returns true if str ends with suffix
bool contains(const std::string& str, const std::string fix); ///< Returns true if str contains fix
std::string to_lower(std::string str); // Converts a string to all lower case

#endif // Utilities_h__
