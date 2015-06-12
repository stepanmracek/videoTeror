cmake_minimum_required(VERSION 2.8.12)
project(videoteror)

ADD_DEFINITIONS(-std=c++11)

option (VTSENSE_EXTRAS "Compile additional extras" OFF)
option (VTSENSE_FACE "Compile face module" OFF)
option (VTSENSE_APPS "Compile apps" OFF)

add_subdirectory(vtsense)
#add_subdirectory(dlib)

if (VTSENSE_EXTRAS)
	add_subdirectory(vtsenseExtras)
endif()

if (VTSENSE_FACE)
	add_subdirectory(face/src)
endif()

if (VTSENSE_APPS  AND VTSENSE_EXTRAS)
	add_subdirectory(apps)
endif()
