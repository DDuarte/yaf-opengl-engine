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

std::vector<TiXmlElement*> GetAllChildren(TiXmlElement* root)
{
    std::vector<TiXmlElement*> children;

    if (root)
    {
        if (TiXmlElement* first = root->FirstChildElement())
        {
            children.push_back(first);

            TiXmlElement* next = first->NextSiblingElement();

            while (next)
            {
                children.push_back(next);
                next = next->NextSiblingElement();
            }
        }
    }

    return children;
}

TiXmlElement* GetChildren(TiXmlElement* root, const std::string& name, const std::string& prefix, bool required /*= true*/)
{
    TiXmlElement* ele = root->FirstChildElement(name);
    if (!ele && required)
        throw YafParsingException("<" + prefix + " " + name + "> not found: line: " + std::to_string((long long)root->Row()) + " col: " + std::to_string((long long)root->Column()));

    return ele;
}

TiXmlElement* GetChildren(TiXmlDocument* root, const std::string& name, const std::string& prefix, bool required /*= true*/)
{
    TiXmlElement* ele = root->FirstChildElement(name);
    if (!ele && required)
        throw YafParsingException("<" + prefix + " " + name + "> not found: line: " + std::to_string((long long)root->Row()) + " col: " + std::to_string((long long)root->Column()));

    return ele;
}
