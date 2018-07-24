project(vats)
# name: Voice Auto Test Server

cmake_minimum_required(VERSION 2.8)

set( CMAKE_BUILD_TYPE Debug )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libstdc++ -pthread -Wall -Wextra")
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/contribute/vad)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/.
    /usr/local/include/opencv2/
    )


find_package(Threads)

add_subdirectory(./contribute/pugixml/scripts)
add_subdirectory(./autotestserver/alsa_audio)
add_subdirectory(./autotestserver/audio_recog)
add_subdirectory(./autotestserver/pic_recog)
add_subdirectory(./autotestserver/testor)


SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/./autotestserver/testor/LOG.h
    )

SET(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/./autotestserver/main.cc
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE "contribute/pugixml/src")
#target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} audio_recog alsa_audio testor pugixml pthread asound jsoncpp fftw3 vad)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} audio_recog alsa_audio pic_recog testor pugixml pthread asound jsoncpp fftw3)
#target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} alsa_audio pugixml pthread asound)
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
message("thread: " ${CMAKE_THREAD_LIBS_INIT})
message("CXX:    " ${CMAKE_CXX_FLAGS})
