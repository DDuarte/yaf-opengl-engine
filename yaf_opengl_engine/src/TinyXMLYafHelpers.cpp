#include "TinyXMLYafHelpers.h"

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

TiXmlElement* GetChildren(TiXmlElement* root, const std::string& name, const std::string& prefix, bool required /*= true*/)
{
    TiXmlElement* ele = root->FirstChildElement(name);
    if (!ele)
        throw YafParsingException("<" + prefix + " " + name + "> not found");

    return ele;
}

TiXmlElement* GetChildren(TiXmlDocument* root, const std::string& name, const std::string& prefix, bool required /*= true*/)
{
    TiXmlElement* ele = root->FirstChildElement(name);
    if (!ele)
        throw YafParsingException("<" + prefix + " " + name + "> not found");

    return ele;
}
