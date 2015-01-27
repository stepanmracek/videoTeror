cmake_minimum_required(VERSION 2.8.12)
project(demo)

file(GLOB_RECURSE ${PROJECT_NAME}.Sources "*.cpp")
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}.Sources})

target_link_libraries(${PROJECT_NAME}
	vtsense
)