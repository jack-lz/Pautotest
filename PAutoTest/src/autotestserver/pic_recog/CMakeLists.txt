PROJECT(pic_recog)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/.
    /usr/local/include/opencv2/
    ../testor/
    )


#FIND_LIBRARY(RUNTIME_LIB rt /usr/lib  /usr/local/lib NO_DEFAULT_PATH)

#find_package(OpenCV REQUIRED)

link_libraries("/usr/local/lib/libopencv_core.so"
               "/usr/local/lib/libopencv_highgui.so"
               "/usr/local/lib/libopencv_imgcodecs.so"
               "/usr/local/lib/libopencv_videoio.so"
               "/usr/local/lib/libopencv_imgproc.so"
               "/usr/local/lib/libopencv_calib3d.so"
               "/usr/local/lib/libopencv_features2d.so"
               "/usr/local/lib/libopencv_xfeatures2d.so"
               "/usr/lib/libtesseract.so"
               "/usr/lib/liblept.so"
               )


SET(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/./ExtractImageFromVideo.h
    ${PROJECT_SOURCE_DIR}/./ExtractPointsFromImage.h
    ${PROJECT_SOURCE_DIR}/./ImageRecogInfo.h
    ${PROJECT_SOURCE_DIR}/./PicRecogProcess.h
    ${PROJECT_SOURCE_DIR}/./RecogCharWithOCR.h
    ${PROJECT_SOURCE_DIR}/./RectifyImages.h
    )

SET(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/./ExtractImageFromVideo.cpp
    ${PROJECT_SOURCE_DIR}/./ExtractPointsFromImage.cpp
    ${PROJECT_SOURCE_DIR}/./ImageRecogInfo.cpp
    ${PROJECT_SOURCE_DIR}/./PicRecogProcess.cpp
    ${PROJECT_SOURCE_DIR}/./RecogCharWithOCR.cpp
    ${PROJECT_SOURCE_DIR}/./RectifyImages.cpp
    )

add_library(pic_recog ${SOURCE_FILES})
target_include_directories(pic_recog PUBLIC ${PROJECT_SOURCE_DIR} PRIVATE "../pic_recog")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT} pthread)
