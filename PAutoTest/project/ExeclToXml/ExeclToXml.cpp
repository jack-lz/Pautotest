#include <cstdio>  
#include <iostream> 
#include "ExeclToXml.h"

ExeclToXml::ExeclToXml() {}
ExeclToXml::~ExeclToXml() {}

bool ExeclToXml::getXml(const std::string &execlPath)
{
    char line[300];  
    FILE *fp;  
    std::string cmd = "./pyTra.py " + execlPath;  
    // system call  
    const char *sysCommand = cmd.data();  
    if ((fp = popen(sysCommand, "r")) == NULL) {  
        std::cout << "file not exist" << std::endl;  
        return false;  
    }  
    while (fgets(line, sizeof(line)-1, fp) != NULL) {}  
    pclose(fp);
    return true;  
}  
