#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <string>
// #include "RecordButton_Define.h"
class Action
{
protected:
    int _timeout;
    std::string _final;

public:
    Action(int timeout)
        :_timeout(timeout)
        ,_final("")
    {}

    Action(int timeout, std::string final)
        :_timeout(timeout)
        ,_final(final)
    {}

    virtual ~Action() {}

    virtual bool run(std::string casename, int seq) = 0;
    virtual std::string get_final() const;
};
typedef std::shared_ptr<Action> spAction;

#endif /* ACTION_H */
