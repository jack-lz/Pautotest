#include <iostream>

#include "TestConfigParser.h"
#include "LOG.h"

 std::list<std::pair<std::string,std::string>> TestConfigParser::Parse(const char* testConfigFile) {

    pugi::xml_document xmlDoc;

    std::list<std::pair<std::string,std::string>> configs;
    if(!xmlDoc.load_file(testConfigFile)) {
        LOG(" ERROR: read %s failed!\n", testConfigFile);
        return configs;
    }

    pugi::xml_node nodeConfigs = xmlDoc.child("case_config");
    for (pugi::xml_node nodeConfig : nodeConfigs.children("test_case")) {
        std::string type = nodeConfig.child_value("type");
        std::string caseName = nodeConfig.child_value("case_name");
        std::pair<std::string,std::string> p(type,caseName);
        configs.push_back(p);
    }
    return configs;
}
