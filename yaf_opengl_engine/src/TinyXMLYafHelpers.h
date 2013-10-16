#ifndef TinyXMLYafHelpers_h__
#define TinyXMLYafHelpers_h__

#include <vector>
#include <string>
#include <tinyxml.h>

#include "YafEnums.h"

std::vector<TiXmlElement*> GetAllChildren(TiXmlElement* root, const std::string& name);
std::vector<TiXmlElement*> GetAllChildren(TiXmlElement* root);

TiXmlElement* GetChildren(TiXmlElement* root, const std::string& name, const std::string& prefix, bool required = true);
TiXmlElement* GetChildren(TiXmlDocument* root, const std::string& name, const std::string& prefix, bool required = true);

template<typename T>
inline T GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required = true)
{
    T res;
    if (element->QueryValueAttribute(name, &res) != TIXML_SUCCESS)
        if (required)
            throw YafParsingException("<" + prefix + " " + name + "> not found: line: " + std::to_string((long long)element->Row()) + " col: " + std::to_string((long long)element->Column()));
        else
            return T();
    return res;
}

template<>
inline bool GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return BoolFromString(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
inline YafRGBA GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafRGBA(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
inline YafXY GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafXY(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
inline YafXYZ GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafXYZ(GetAttribute<std::string>(element, name, prefix, required));
}

#endif // TinyXMLYafHelpers_h__
