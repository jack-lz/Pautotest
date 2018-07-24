#ifndef GEN_TEST_CONFIG_H
#define GEN_TEST_CONFIG_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif
#include <string>

class GenTestConfig
{
public:
    GenTestConfig();
    ~GenTestConfig();
    bool getXml(const std::string &workSpacePath);

private:
}; 

#endif
