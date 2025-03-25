#ifndef _embedder_src_engine_classes_h
#define _embedder_src_engine_classes_h

#include <map>
#include <string>
#include <any>

class ClassData
{
private:
    std::map<std::string, std::any> m_classData;
public:
};

#endif