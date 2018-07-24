#ifndef TESTCONFIGPARSER_H
#define TESTCONFIGPARSER_H

#include <memory>
#include <string>
#include <list>
#include <utility>

#include "pugixml.hpp"

class TestConfigParser {
public:
    TestConfigParser(){};
    ~TestConfigParser(){};

    std::list<std::pair<std::string,std::string>> Parse(const char* testConfigFile);

};

#endif // TESTCONFIGPARSER_H
