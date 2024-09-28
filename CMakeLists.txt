
cmake_minimum_required(VERSION 3.26)
project(LabSoundCDemo)

# Don't report that sample file installation up to date
set(CMAKE_INSTALL_MESSAGE LAZY)

set(LABSOUNDCDEMO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("${LABSOUNDCDEMO_ROOT}/LabSoundDemo.config.h"
               "${LABSOUNDCDEMO_ROOT}/LabSoundDemo.h" @ONLY)

# Find LabSound and dependencies
find_package(LabSound QUIET)

# if LabSound is not found, use FetchContent to download it
if(NOT LabSound_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    LabSound
    GIT_REPOSITORY "http://github.com/LabSound/LabSound.git"
    GIT_TAG "main"
    )
    message(STATUS "Fetching LabSound from source")
    FetchContent_MakeAvailable(LabSound)
    message(STATUS "LabSound found at: ${LabSound_SOURCE_DIR}")
endif()

if(APPLE)
  set(PLATFORM_LIBS
        "-framework AudioToolbox"
        "-framework AudioUnit"
        "-framework Accelerate"
        "-framework Cocoa"
        "-framework CoreAudio"
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        )
endif()

add_executable(LabSoundCDemo
    labsound-c.h labsound-c.cpp
    tinycthread.h tinycthread.c
    LabSoundCDemo.c flecs.h flecs.c)

set_property(TARGET LabSoundCDemo PROPERTY C_STANDARD 11)
set_property(TARGET LabSoundCDemo PROPERTY CXX_STANDARD 17)
target_link_libraries(LabSoundCDemo
    LabSound::LabSound
    LabSoundRtAudio::LabSoundRtAudio
    ${PLATFORM_LIBS})
target_include_directories(LabSoundCDemo PRIVATE "${LABSOUNDDEMO_ROOT}")
install(TARGETS LabSoundCDemo RUNTIME DESTINATION bin)

# Copy the sample files from LabSound sources, src/assets/samples to
# assets/samples in the run time bin directory
message(STATUS "Copying sample files from ${LabSound_SOURCE_DIR}/assets/samples")
file(GLOB SAMPLES "${LabSound_SOURCE_DIR}/assets/samples/*")
foreach(SAMPLE ${SAMPLES})
    message(STATUS "Copying sample file: ${SAMPLE}")
    get_filename_component(SAMPLE_NAME ${SAMPLE} NAME)

    # if the sample is a directory, copy the directory and its contents
    if(IS_DIRECTORY ${SAMPLE})
        # into the binary dir for easy debugging
        file(COPY ${SAMPLE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/samples)
        # if installing, copy the directory and its contents
        install(DIRECTORY ${SAMPLE} DESTINATION bin/assets/samples)
    else()
        # into the binary dir for easy debugging
        configure_file(${SAMPLE} ${CMAKE_CURRENT_BINARY_DIR}/assets/samples/${SAMPLE_NAME} COPYONLY)
        # if installing, copy the file
        install(FILES ${SAMPLE} DESTINATION bin/assets/samples)
    endif()
endforeach()
