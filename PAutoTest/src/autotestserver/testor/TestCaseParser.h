#ifndef TESTCASEPARSER_H
#define TESTCASEPARSER_H

#include <list>
#include <memory>

#include "pugixml.hpp"

#include "Action.h"
#include "SocketServer.h"
#include <map>
#include <vector>

class TestCase;

class TestCaseParser
{
private:
    SocketServer &_ss;
    spAction ParseAction(pugi::xml_node& nodeAction);

public:
    TestCaseParser(SocketServer &ss)
        :_ss(ss)
    {}

    std::list<std::shared_ptr<TestCase>> Parse(const char* testCaseFile);
    bool createXML(const char *reportFile);
    bool toXML(const char* reportFile, std::map<std::string, std::vector<std::string>> report);
    bool configXML(const char *reportFile);
};

#endif /* TESTCASEPARSER_H */
