PROJECT(audio_recog)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/.
    /usr/include/jsoncpp
    ../testor/
    )

SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/../testor/LOG.h
    ${PROJECT_SOURCE_DIR}/./audio_process.h
    ${PROJECT_SOURCE_DIR}/./fingerprint.h
    ${PROJECT_SOURCE_DIR}/./hash.h
    ${PROJECT_SOURCE_DIR}/./RecordButton_Define.h
    ${PROJECT_SOURCE_DIR}/./WavFile.h
    ${PROJECT_SOURCE_DIR}/./wav.h
    ${PROJECT_SOURCE_DIR}/Audio_Recog.h
    ${PROJECT_SOURCE_DIR}/recog.h
    ${PROJECT_SOURCE_DIR}/vad.h
#    ${PROJECT_SOURCE_DIR}/ConfigAna.h
    )

SET(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/./audio_process.cpp
    ${PROJECT_SOURCE_DIR}/./Audio_Recog.cpp
    ${PROJECT_SOURCE_DIR}/./fingerprint.cpp
    ${PROJECT_SOURCE_DIR}/./hash.cpp
    ${PROJECT_SOURCE_DIR}/./recog.cpp
    ${PROJECT_SOURCE_DIR}/./wav.cpp
    ${PROJECT_SOURCE_DIR}/./WavFile.cpp
    ${PROJECT_SOURCE_DIR}/./vad.cpp
#    ${PROJECT_SOURCE_DIR}/./ConfigAna.cpp
    )

add_library(audio_recog ${SOURCE_FILES})
#target_include_directories(audio_recog PUBLIC ${PROJECT_SOURCE_DIR} PRIVATE "../alsa_audio")
target_include_directories(audio_recog PUBLIC ${PROJECT_SOURCE_DIR} PRIVATE "../alsa_audio" "../testor")
#target_link_libraries(audio_recog LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} jsoncpp fftw3 vad pthread )
target_link_libraries(audio_recog LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} jsoncpp fftw3 pthread )

#ADD_CUSTOM_TARGET(testor_headers SOURCES ${HEADER_FILES})
