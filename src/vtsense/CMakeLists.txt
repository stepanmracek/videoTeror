cmake_minimum_required(VERSION 2.8.12)
project(vtsense)

ADD_DEFINITIONS(-std=c++11)

file(GLOB_RECURSE ${PROJECT_NAME}.Sources "src/*.cpp")
file(GLOB_RECURSE ${PROJECT_NAME}.Headers "include/*.h")
add_library (${PROJECT_NAME} SHARED ${${PROJECT_NAME}.Sources} ${${PROJECT_NAME}.Headers})

target_include_directories(${PROJECT_NAME}
	PUBLIC include
	PRIVATE src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC opencv_core
	PUBLIC opencv_highgui
	PUBLIC opencv_imgproc
	PUBLIC opencv_ml
	PUBLIC opencv_objdetect
	PUBLIC opencv_features2d
	PUBLIC opencv_video
)
