#ifndef EXECL_TO_XML_H
#define EXECL_TO_XML_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif
#include <string>

class ExeclToXml
{
public:
    ExeclToXml();
    ~ExeclToXml();
    bool getXml(const std::string &workSpacePath);

private:
}; 

#endif
