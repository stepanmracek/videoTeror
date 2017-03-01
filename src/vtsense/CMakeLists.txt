cmake_minimum_required(VERSION 2.8.12)
project(vtsense)

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}.Sources "src/*.cpp")
file(GLOB_RECURSE ${PROJECT_NAME}.Headers "include/*.h")
add_library (${PROJECT_NAME} SHARED ${${PROJECT_NAME}.Sources} ${${PROJECT_NAME}.Headers})

target_include_directories(${PROJECT_NAME}
	PUBLIC include
	PRIVATE src
	PRIVATE "../"
)

target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS}
	#dlib
)

install(TARGETS ${PROJECT_NAME}	LIBRARY DESTINATION lib)
install(DIRECTORY include/vtsense DESTINATION include)
