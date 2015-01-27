cmake_minimum_required(VERSION 2.8.12)
project(faceDetection)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

file(GLOB_RECURSE ${PROJECT_NAME}.Sources "*.cpp")
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}.Sources})

target_link_libraries(${PROJECT_NAME}
    vtsense
    vtsenseExtras
    F3D.faceCommon
    ${QT_LIBRARIES}
)
