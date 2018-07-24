#ifndef TESTCASE_H
#define TESTCASE_H

#include <string>
#include <list>
#include <memory>

#include "Action.h"
class TestCase
{
private:
    const std::string _name;
    const std::string _type;
    std::list<spAction> _actions;
    spAction _final_action = NULL;

public:
    TestCase(std::string name,std::string type)
        :_name(name)
        ,_type(type)
    {}

    std::string name() { return _name; }
    std::string type() { return _type; }
    void add(spAction action) {
        _actions.push_back(action);
    }

    bool Run();

    void setFinal_action(spAction final_action);
};

typedef std::shared_ptr<TestCase> spTestCase;

#endif /* TESTCASE_H */
