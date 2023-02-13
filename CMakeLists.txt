
cmake_minimum_required (VERSION 3.4)
project(LabSoundCDemo)

# Don't report that sample file installation up to date
set(CMAKE_INSTALL_MESSAGE LAZY)

set(LABSOUNDCDEMO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("${LABSOUNDCDEMO_ROOT}/LabSoundDemo.config.h" "${LABSOUNDCDEMO_ROOT}/LabSoundDemo.h" @ONLY)

find_package(libnyquist REQUIRED)
find_package(LabSound REQUIRED)
find_package(LabSoundRtAudio REQUIRED)

if (APPLE)
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
