PROJECT(testor)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/.
    /usr/include/jsoncpp
    /usr/local/include/opencv2/
    )

SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/./LOG.h
    ${PROJECT_SOURCE_DIR}/./Action.h
    ${PROJECT_SOURCE_DIR}/./ActionVrControl.h
    ${PROJECT_SOURCE_DIR}/./ActionPlaying.h
    ${PROJECT_SOURCE_DIR}/./ActionComparePrompt.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeTTSAndViewParallel.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeVrView.h
    ${PROJECT_SOURCE_DIR}/./Controler.h
    ${PROJECT_SOURCE_DIR}/./TestCaseParser.h
    ${PROJECT_SOURCE_DIR}/./TestCase.h
    ${PROJECT_SOURCE_DIR}/./TestConfigParser.h
    ${PROJECT_SOURCE_DIR}/./sockcomm.h
    ${PROJECT_SOURCE_DIR}/./SocketServer.h
    ${PROJECT_SOURCE_DIR}/./ExeclToXml.h
    ${PROJECT_SOURCE_DIR}/./GenTestConfig.h
    ${PROJECT_SOURCE_DIR}/./ConfigAna.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeepPromptBeep.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeepPrompt.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgePromptBeep.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeEmptyRecordFile.h
    ${PROJECT_SOURCE_DIR}/./GetBeepLen.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeep.h
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeepBeep.h
    ${PROJECT_SOURCE_DIR}/./ActionDelayed.h
    )

SET(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/./LOG.cpp
    ${PROJECT_SOURCE_DIR}/./Action.cpp
    ${PROJECT_SOURCE_DIR}/./ActionVrControl.cpp
    ${PROJECT_SOURCE_DIR}/./ActionPlaying.cpp
    ${PROJECT_SOURCE_DIR}/./ActionComparePrompt.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeTTSAndViewParallel.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeVrView.cpp
    ${PROJECT_SOURCE_DIR}/./Controler.cpp
    ${PROJECT_SOURCE_DIR}/./TestCaseParser.cpp
    ${PROJECT_SOURCE_DIR}/./TestCase.cpp
    ${PROJECT_SOURCE_DIR}/./TestConfigParser.cpp
    ${PROJECT_SOURCE_DIR}/./SocketServer.cpp
    ${PROJECT_SOURCE_DIR}/./ExeclToXml.cpp
    ${PROJECT_SOURCE_DIR}/./GenTestConfig.cpp
    ${PROJECT_SOURCE_DIR}/./ConfigAna.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeepPromptBeep.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgePromptBeep.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeepPrompt.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeEmptyRecordFile.cpp
    ${PROJECT_SOURCE_DIR}/./GetBeepLen.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeep.cpp
    ${PROJECT_SOURCE_DIR}/./ActionJudgeBeepBeep.cpp
    ${PROJECT_SOURCE_DIR}/./ActionDelayed.cpp
    )

#LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../pic_recog)

#LINK_LIBRARIES(pic_recog)

add_library(testor ${SOURCE_FILES})
target_include_directories(testor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE "../alsa_audio" "../audio_recog" "../pic_recog" "../../contribute/pugixml/src")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} pic_recog)

#ADD_CUSTOM_TARGET(testor_headers SOURCES ${HEADER_FILES})
