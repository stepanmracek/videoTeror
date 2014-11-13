cmake_minimum_required(VERSION 2.8.12)
project(vtsense)

find_package(OpenCV REQUIRED)
find_package(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

file(GLOB_RECURSE vtsenseSources *.cpp)
add_library(${PROJECT_NAME} SHARED ${vtsenseSources})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	${QT_LIBRARIES}
	${OpenCV_LIBS}
)