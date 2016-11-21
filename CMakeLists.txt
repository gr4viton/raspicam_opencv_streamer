#####################################
cmake_minimum_required (VERSION 2.8) 
project (raspicam_test)
find_package(raspicam REQUIRED)
find_package(OpenCV)
IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")
add_executable (raspicam_opencv_streamer raspicam_opencv_streamer.cpp)  
#add_executable (video-write video-write.cpp)
target_link_libraries (raspicam_opencv_streamer ${raspicam_CV_LIBS})
#target_link_libraries (video-write ${raspicam_CV_LIBS})

ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()
#####################################
 
