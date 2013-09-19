#ifndef TinyXMLYafHelpers_h__
#define TinyXMLYafHelpers_h__

#include <vector>
#include <string>
#include <tinyxml.h>

#include "YafEnums.h"

std::vector<TiXmlElement*> GetAllChildren(TiXmlElement* root, const std::string& name)
{
    std::vector<TiXmlElement*> children;

    if (root)
    {
        if (TiXmlElement* first = root->FirstChildElement(name))
        {
            children.push_back(first);

            TiXmlElement* next = first->NextSiblingElement(name);

            while (next)
            {
                children.push_back(next);
                next = next->NextSiblingElement(name);
            } 
        }
    }

    return children;
}

template<typename T>
T GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required = true)
{
    T res;
    if (element->QueryValueAttribute(name, &res) != TIXML_SUCCESS)
        if (required)
            throw YafParsingException("<" + prefix + " " + name + "> not found");
        else
            return T();
    return res;
}

template<>
bool GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return BoolFromString(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
YafRGBA GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafRGBA(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
YafXY GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafXY(GetAttribute<std::string>(element, name, prefix, required));
}

template<>
YafXYZ GetAttribute(const TiXmlElement* element, const std::string& name, const std::string& prefix, bool required /* = true */)
{
    return YafXYZ(GetAttribute<std::string>(element, name, prefix, required));
}

#endif // TinyXMLYafHelpers_h__
