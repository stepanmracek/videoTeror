cmake_minimum_required(VERSION 2.8.12)
project(vtsenseExtras)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

file(GLOB_RECURSE ${PROJECT_NAME}.Sources "src/*.cpp")
add_library (${PROJECT_NAME} SHARED ${${PROJECT_NAME}.Sources})

target_include_directories(${PROJECT_NAME}
	PUBLIC include
	PRIVATE src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC vtsense
)
