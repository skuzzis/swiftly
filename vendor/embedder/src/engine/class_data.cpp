#include "classes.h"

ClassData::ClassData(std::map<std::string, std::any> data)
{
    m_classData = data;
}

void ClassData::SetData(std::string key, std::any value)
{
    m_classData[key] = value;
}