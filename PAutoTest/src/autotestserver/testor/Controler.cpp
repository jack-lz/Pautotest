#include <iostream>
#include <unistd.h>

#include "Controler.h"
#include "SocketServer.h"
#include "TestCase.h"
#include "TestCaseParser.h"
#include "ExeclToXml.h"
#include "GenTestConfig.h"
#include "TestConfigParser.h"
#include "ConfigAna.h"
#include "LOG.h"


using namespace std;

void Controler::Run(const char *workSpacePath, const char *reportFile)
{
//    while (1) {
//        LOG("please adjust the camera!\n");
//        ImageRecogInfo::GetInstance()->setVideoflag(true);
//        std::thread startCamera(&Controler::ShowCameraVideo,this);

//        LOG(if has adjusted the camera! please input command!\n");
//        LOG("0 : EXIT!\n");
//        LOG("1 : Start check the video position!\n");

//        string strInput("");
//        bool extract_flag = false;
//        while(1) {
//            strInput = "";
//            cin.clear();
//            cin.sync();
//            cin >> strInput;
//            LOG("your command is %s\n", strInput.c_str();
//            if ("0" == strInput) {
//                // stop startCamera thread
//                ImageRecogInfo::GetInstance()->setVideoflag(false);
//                startCamera.join();
//                return;
//            }
//            else if ("1" == strInput) {
//                // stop startCamera thread
//                ImageRecogInfo::GetInstance()->setVideoflag(false);
//                startCamera.join();
////                    cv::namedWindow("imageROI", WINDOW_NORMAL);
////                    cv::imshow("imageROI",  m_videoshower.getFrame());
////                    waitKey(0);
//                if (m_videoshower.getFrame().empty()) {
//                    LOG("请确认摄像头已经连接且没有被其他程序占用! %s\n",strInput.c_str());
//                    extract_flag = false;
//                    break;
//                }
//                m_videoshower.getPoint().ExtractElements(m_videoshower.getFrame());
//                if (m_videoshower.getPoint().getCorners().size() != 4) {
//                    LOG("The camera position isn't right!\n");
//                    extract_flag = false;
//                    break;
//                }
//                else {
//                    ImageRecogInfo::GetInstance()->setPoint(m_videoshower.getPoint());
//                    extract_flag = true;
//                    break;
//                }
//            }
//            else {
//                LOG("The command isn't right! please input again!\n");
//                continue;
//            }
//        }
//        if (true == extract_flag) {
//            LOG("ExtractElements is succeed!\n");
//            break;
//        }
//    }
    std::string sTestCaseFile = std::string(workSpacePath) + "/testFile/out.xml";
    std::string testConfigFile = std::string(workSpacePath) + "/testFile/testConfig.xml";
    ExeclToXml _execlToXml;
    if (!_execlToXml.getXml(workSpacePath)) {
        return;
    }

    GenTestConfig _genTestConfig;
    if (!_genTestConfig.getXml(workSpacePath)) {
        return;
    }

    SocketServer _socketServer;
    if (! _socketServer.start()) {
        return;
    }

//    std::map<std::string, std::string> report;//暂定，如需扩展，value的string改为list
    std::map<std::string, std::vector<std::string>> report;
    std::vector<std::string> resultList;
    std::list<std::pair<std::string,std::string>> configList;
    TestConfigParser configParser;
    configList = configParser.Parse(testConfigFile.c_str());

    TestCaseParser parser(_socketServer);
    std::list<std::shared_ptr<TestCase> > cases = parser.Parse(sTestCaseFile.c_str());
    if (cases.empty()) {
        //LOG("ERROR: test cases is EMPTY!\n");
        return;
    }

    std::list<std::shared_ptr<TestCase>> runCases;
    if (configList.empty() == 1) {
        for (auto testCase : cases) {
            runCases.push_back(testCase);
        }
    } else {
        for (auto config : configList) {
            for (auto testCase : cases) {
                if ((config.second.compare(testCase->name()) == 0) || (config.first.compare(testCase->type()) == 0)) {
                    std::list<std::shared_ptr<TestCase>>::iterator iter = find(runCases.begin(), runCases.end(), testCase);
                    if (iter == runCases.end()) {
                        runCases.push_back(testCase);
                    }
                }
            }
        }
    }



    parser.createXML(reportFile);

    for (auto testCase : runCases) {
        LOG("%s is running! \n",testCase->name().c_str());
        bool result = testCase->Run();
        sleep(5);
        int i = 0;

        if (!result){
            for (i = 1;i <= g_configInfo.times;++i) {
                result = testCase->Run();
                sleep(5);
                if (result) {
                    break;
                }
            }
        }
        resultList.push_back(std::to_string(i));

        if (result) {
            resultList.push_back(std::string("success"));
            report.insert(std::make_pair(testCase->name(),resultList));
        }
        else {
            resultList.push_back(std::string("failed"));
            report.insert(std::make_pair(testCase->name(),resultList));
        }
        LOG("################################# %s    %d \n", testCase->name().c_str(), result);

#ifdef LOG_ENABLE
        printf("################################# %s    %d \n", testCase->name().c_str(), result);
#endif

        parser.toXML(reportFile, report);
        report.clear();
        resultList.clear();
    }
    parser.configXML(reportFile);
    //    parser.toXML(reportFile, report);
}

void Controler::ShowCameraVideo()
{
    m_videoshower.ShowCameraVideo();
}





