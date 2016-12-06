//
// Created by Ferdinando Fioretto on 12/3/16.
//

#ifndef CPUBE_XMLUTILS_H
#define CPUBE_XMLUTILS_H

#include <rapidxml.hpp>
#include <string>

using namespace rapidxml;


namespace xmlutils
{
    inline void throwIfNotFound(xml_node<>* node, const std::string& name) {
        if (!node->first_attribute(name.c_str()))
            throw std::invalid_argument("Attribute " + name + " not found");
    }

    inline int getIntAttribute(xml_node<>* node, const std::string& name)
    {
        throwIfNotFound(node, name);
        return std::stoi(node->first_attribute(name.c_str())->value());
    }

    inline double getDoubleAttribute(xml_node<>* node, const std::string& name)
    {
        throwIfNotFound(node, name);
        return std::stod(node->first_attribute(name.c_str())->value());
    }

    inline std::string getStrAttribute(xml_node<>* node, const std::string& name)
    {
        throwIfNotFound(node, name);
        return node->first_attribute(name.c_str())->value();
    }

}
#endif //CPUBE_XMLUTILS_H
