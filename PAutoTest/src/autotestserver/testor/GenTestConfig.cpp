#include <cstdio>
#include <iostream>
#include "GenTestConfig.h"
#include "LOG.h"

#define SCRIPT_PATH "/script/testConfig/pyTestConfig.py"

GenTestConfig::GenTestConfig() {}
GenTestConfig::~GenTestConfig() {}

bool GenTestConfig::getXml(const std::string &workSpacePath)
{
    char line[300];
    FILE *fp;
    std::string cmd = "python " + workSpacePath + SCRIPT_PATH +" " + workSpacePath;
    // system call
    const char *sysCommand = cmd.data();
    if ((fp = popen(sysCommand, "r")) == NULL) {
        LOG("file not exist\n");
        return false;
    }
    while (fgets(line, sizeof(line)-1, fp) != NULL) {}
    pclose(fp);
    return true;
}
