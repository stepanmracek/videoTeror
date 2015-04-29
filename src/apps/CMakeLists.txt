cmake_minimum_required(VERSION 2.8.12)
project(apps)

add_subdirectory(bigGunDetection)
add_subdirectory(bigGunDetectionTraining)
add_subdirectory(demo)
add_subdirectory(gunDetection)
add_subdirectory(motionDetection)
add_subdirectory(peopleDetection)
add_subdirectory(pointTracking)
add_subdirectory(faceDetectionInVideo)

if (VTSENSE_FACE)
	add_subdirectory(faceIdentification)
	add_subdirectory(evaluateFrgc)
	add_subdirectory(nirFace)
	add_subdirectory(faceDetection)
endif()

