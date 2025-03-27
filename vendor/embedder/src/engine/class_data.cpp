#include "classes.h"

ClassData::ClassData(std::map<std::string, std::any> data, std::string className)
{
    m_classData = data;
    m_className = className;
}

void ClassData::SetData(std::string key, std::any value)
{
    m_classData[key] = value;
}

std::string ClassData::GetClassname()
{
    return m_className;
}