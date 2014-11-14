cmake_minimum_required(VERSION 2.8.12)
project(demo)

find_package(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

file(GLOB_RECURSE demoSources *.cpp)
add_executable(${PROJECT_NAME} ${demoSources})

target_link_libraries(${PROJECT_NAME}
	vtsense
	#${QT_LIBRARIES}
)