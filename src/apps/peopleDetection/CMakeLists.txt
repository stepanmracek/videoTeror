cmake_minimum_required(VERSION 2.8.12)
project(peopleDetection)

find_package(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

file(GLOB_RECURSE peopleDetectionSources *.cpp)
add_executable(${PROJECT_NAME} ${peopleDetectionSources})

target_link_libraries(${PROJECT_NAME}
        vtsenseExtras
        ${QT_LIBRARIES}
)
