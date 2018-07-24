#ifndef ACTIONJUDGEEMPTYRECORDFILE_H
#define ACTIONJUDGEEMPTYRECORDFILE_H

#include "Action.h"

class ActionJudgeEmptyRecordFile : public Action
{
private:
    const std::string _filePath;
public:
    ActionJudgeEmptyRecordFile(int timeout, std::string filePath)
        : Action(timeout)
        ,_filePath(filePath)
    {
    }

    virtual ~ActionJudgeEmptyRecordFile()
    {}

    virtual bool run(std::string casename, int seq) override;
};

#endif /* ACTIONJUDGEBEMPTYRECORDFILE_H */
