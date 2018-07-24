PROJECT(alsa_audio_test)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/.
    ../testor/
    )

SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/../testor/LOG.h
    ${PROJECT_SOURCE_DIR}/./audio_output_alsa.h
    ${PROJECT_SOURCE_DIR}/./audio_input_alsa.h
    ${PROJECT_SOURCE_DIR}/./audio_input.h
    ${PROJECT_SOURCE_DIR}/./audio_input_file.h
    )

SET(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/./audio_input_alsa.cc
    ${PROJECT_SOURCE_DIR}/./audio_output_alsa.cc
    ${PROJECT_SOURCE_DIR}/./audio_input_file.cc
    )

add_library(alsa_audio ${SOURCE_FILES})

target_include_directories(alsa_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE "../testor" "../contribute/pugixml/src")

#ADD_EXECUTABLE(alsa_audio_test ${PROJECT_SOURCE_DIR}/main.cc)
#target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} alsa_audio asound pthread)

FILE(GLOB_RECURSE XML_CONFIG "*.xml")
ADD_CUSTOM_TARGET(alsa_audio_headers SOURCES ${HEADER_FILES})

