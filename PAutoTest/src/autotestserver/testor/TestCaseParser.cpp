#include <string>
#include <string.h>
#include <iostream>
#include "TestCaseParser.h"
#include "TestCase.h"
#include "Action.h"
#include "ActionVrControl.h"
#include "ActionPlaying.h"
#include "ActionComparePrompt.h"
#include "ActionJudgeTTSAndViewParallel.h"
#include "ActionJudgeVrView.h"
#include "ActionJudgeEmptyRecordFile.h"
#include "ActionJudgeBeepPrompt.h"
#include "ActionJudgePromptBeep.h"
#include "ActionJudgeBeepPromptBeep.h"
#include "ActionJudgeBeep.h"
#include "ActionJudgeBeepBeep.h"
#include "ActionDelayed.h"
#include "LOG.h"

std::list<std::shared_ptr<TestCase>> TestCaseParser::Parse(const char *testCaseFile)
{
    std::list<std::shared_ptr<TestCase>> cases;
    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_file(testCaseFile)) {
        LOG("ERROR: read %s failed!\n", testCaseFile);
        return cases;
    }

    pugi::xml_node nodeCases = xmlDoc.child("testcases");
    for (pugi::xml_node nodeCase : nodeCases.children("case")) {
        std::string caseName = nodeCase.child("case_head").child_value("name");
        std::string caseType = nodeCase.child("case_head").child_value("type");
//        LOG("case name is %s , case type is %s\n", caseName.c_str(), caseType.c_str());
        spTestCase testCase = std::make_shared<TestCase>(caseName,caseType);
        for (pugi::xml_node nodeAction : nodeCase.children("action")) {
            spAction action = ParseAction(nodeAction);
            if (! action) {
                LOG("ERROR: read %s failed!\n", testCaseFile);
                return std::list<std::shared_ptr<TestCase>>();
            }
            std::string sFinal = action->get_final().c_str();
            if (0 == strcmp("must", action->get_final().c_str())) {
                testCase->setFinal_action(action);
            }
            else {
                testCase->add(action);
            }
        }

        cases.push_back(testCase);
    }

    return cases;
}

bool TestCaseParser::createXML(const char *reportFile)
{
    pugi::xml_document xmlDoc;
    pugi::xml_node noodReport = xmlDoc.append_child("report");

    if (xmlDoc.save_file(reportFile, "\t", 1U, pugi::encoding_utf8)) {\
        return true;
    }
    else {
        LOG("save file failed!\n");
        return false;
    }
}

bool TestCaseParser::toXML(const char *reportFile, std::map<std::string, std::vector<std::string>> report)
{
    pugi::xml_document xmlDoc;
    bool loadRes = xmlDoc.load_file(reportFile);
    if (!loadRes) {
        LOG("ERROR: loadXml %s failed!\n", reportFile);
    }

    pugi::xml_node noodReport = xmlDoc.child("report");
    std::map<std::string, std::vector<std::string>>::iterator it = report.begin();
    while (it != report.end()) {
        pugi::xml_node nodeCase = noodReport.append_child("case");

        pugi::xml_node nodeCaseName = nodeCase.append_child("case_name");
        nodeCaseName.append_child(pugi::node_pcdata).set_value(it->first.c_str());

        pugi::xml_node nodeCaseResult = nodeCase.append_child("case_result");
        nodeCaseResult.append_child(pugi::node_pcdata).set_value(it->second[1].c_str());

        pugi::xml_node nodeCaseExRunTimes = nodeCase.append_child("case_extra_run_times");
        nodeCaseExRunTimes.append_child(pugi::node_pcdata).set_value(it->second[0].c_str());

//        pugi::xml_node nodeCaseResult = nodeCase.append_child("case_result");
//        nodeCaseResult.append_child(pugi::node_pcdata).set_value(it->second.c_str());
        ++it;
    }

    if (xmlDoc.save_file(reportFile, "\t", 1U, pugi::encoding_utf8)) {\
        return true;
    }
    else {
        LOG("save file failed! \n");
        return false;
    }

}

bool TestCaseParser::configXML(const char *reportFile)
{
    pugi::xml_document xmlDoc;
    bool loadRes = xmlDoc.load_file(reportFile);
    if (!loadRes) {
        LOG("ERROR: loadXml %s failed!\n", reportFile);
    }

    pugi::xml_node xlsc = xmlDoc.prepend_child(pugi::node_declaration);
    xlsc.set_name("xml-stylesheet");
    xlsc.append_attribute("type")="text/xsl";
    xlsc.append_attribute("href")="result_xml2xsl.xsl";

    pugi::xml_node pre = xmlDoc.prepend_child(pugi::node_declaration);
    pre.append_attribute("version") = "1.0";
    pre.append_attribute("encoding") = "utf-8";

    if (xmlDoc.save_file(reportFile, "\t", 1U, pugi::encoding_utf8)) {\
        return true;
    }
    else {
        LOG("save file failed! \n");
        return false;
    }
}

spAction TestCaseParser::ParseAction(pugi::xml_node& nodeAction)
{
    std::string type = nodeAction.child_value("type");
    std::string timeout = nodeAction.child_value("timeout");
    std::string final = nodeAction.attribute("final").value();
    std::string beeptypeStart = nodeAction.child("type").attribute("beeptypeStart").value();
    std::string beeptypeEnd = nodeAction.child("type").attribute("beeptypeEnd").value();
    std::string beeptype = nodeAction.child("type").attribute("beeptype").value();

    int t = std::stoi(timeout);
    if ("start_vr" == type) {
        return std::make_shared<ActionVrStart>(t,_ss);
    }
    else if ("stop_vr" == type) {
        return std::make_shared<ActionVrStop>(t,_ss, final);
    }
    else if ("stop_vr_L1" == type) {
        return std::make_shared<ActionVrStopL1>(t,_ss, final);
    }
    else if ("wait_beep" == type) {
		std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionJudgeBeep>(t,filePath,std::atoi(beeptype.c_str()));
    }
    else if ("on_hook" == type) {
        return std::make_shared<ActionHookOn>(t,_ss);
    }
    else if ("longPress_PTT" == type) {
        return std::make_shared<ActionLongPressPTT>(t,_ss);
    }
    else if ("volume_up" == type) {
        return std::make_shared<ActionVolumeUp>(t,_ss);
    }
    else if ("play_voice_command" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionPlaying>(t, filePath);
    }
    else if ("judge_vr_prompt" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionComparePrompt>(t, filePath);
    }
    else if ("judge_tts_and_view_parallel" == type) {
        std::string audioType = nodeAction.child("audio_file").child_value("audio_type");
        std::string audioFilePath = nodeAction.child("audio_file").child_value("file_path");
        std::string audioTimeout = nodeAction.child("audio_file").child_value("timeout");
        int long at = atoi(audioTimeout.c_str());

        std::string picFilePath = nodeAction.child("pic_file").child_value("pic_path");
        std::string picTimeout = nodeAction.child("pic_file").child_value("timeout");
        int long pt = atoi(picTimeout.c_str());

        return std::make_shared<ActionJudgeTTSAndViewParallel>(audioType, audioFilePath, at, picFilePath, pt);
    }
    else if ("judge_vr_view" == type ) {
        std::string filePath = nodeAction.child("pic_file").child_value("pic_path");
        LOG("judge_vr_view is empty now.\n");
        return std::make_shared<ActionJudgeVrView>(t,filePath);
    }
    else if ("judge_vr_beep_prompt_beep" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionJudgeBeepPromptBeep>(t, filePath,std::atoi(beeptypeStart.c_str()), std::atoi(beeptypeEnd.c_str()) );
    }
    else if ("judge_vr_beep_prompt" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionJudgeBeepPrompt>(t, filePath,std::atoi(beeptype.c_str()) );
    }
    else if ("judge_vr_prompt_beep" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionJudgePromptBeep>(t, filePath,std::atoi(beeptype.c_str()) );
    }
    else if ("judge_vr_empty" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");
        return std::make_shared<ActionJudgeEmptyRecordFile>(t, filePath);
    }
    else if ("Judge_vr_beep_beep" == type) {
        std::string filePath = nodeAction.child("audio_file").child_value("file_path");

        return std::make_shared<ActionJudgeBeepBeep>(t, filePath,std::atoi(beeptypeStart.c_str()), std::atoi(beeptypeEnd.c_str()) );

    }
    else if ("wait_seconds" == type) {
        int seconds = atoi(nodeAction.child_value("timeout"));
        return std::make_shared<ActionDelayed>(t, seconds);
    }
    else {
        LOG("ERROR: parse action %s is NOT supported!\n",type.c_str());
        return nullptr;
    }
}
