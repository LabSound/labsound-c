
cmake_minimum_required(VERSION 3.26)
project(LabSoundCDemo)

# Don't report that sample file installation up to date
set(CMAKE_INSTALL_MESSAGE LAZY)

# add an option to build the python bindings
option(BUILD_PYTHON_BINDINGS "Build the python bindings" ON)

set(LABSOUNDCDEMO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("${LABSOUNDCDEMO_ROOT}/LabSoundDemo.config.h"
               "${LABSOUNDCDEMO_ROOT}/LabSoundDemo.h" @ONLY)

# force an optimized release build unless a build type was explicit
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Find LabSound and dependencies
find_package(LabSound QUIET)

# if LabSound is not found, use FetchContent to download it
if(NOT TARGET LabSound::LabSound)
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

find_package(libnyquist REQUIRED)
find_package(LabSoundRtAudio REQUIRED)
#find_package(LabSoundMiniAudio REQUIRED)

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

# C Demo
#
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

# Python bindings
#
# If the option is set, build the python bindings.
# Point the includes at nanobind, find python, add pylabsound.cpp
# and link against LabSound and LabSoundRtAudio and the python library

if(BUILD_PYTHON_BINDINGS)
    find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter
                            Development Development.Module)

    # note: nanobind does not add the Python3_INCLUDE_DIRS to the target
    # include directories, so we must do it manually
    # It also doesn't have an option to supply the directory, so we must
    # set it globally.
    include_directories(${Python3_INCLUDE_DIRS})

    # Find nanobind, and use FetchContent to download it if not found
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/_deps/nanobind-src)

    if(NOT TARGET nanobind AND 0)
        include(FetchContent)
        FetchContent_Declare(
            nanobind
            GIT_REPOSITORY "https://github.com/wjakob/nanobind"
            GIT_TAG "master"
            )
        message(STATUS "Fetching nanobind from source")
        FetchContent_MakeAvailable(nanobind)
        message(STATUS "nanobind found at: ${nanobind_SOURCE_DIR}")
    endif()

    nanobind_add_module(pylabsound
        pylabsound.cpp
        labsound-c.h labsound-c.cpp
        tinycthread.h tinycthread.c
        flecs.h flecs.c)
    target_link_libraries(pylabsound PRIVATE
        LabSound::LabSound
        LabSoundRtAudio::LabSoundRtAudio
        ${Python3_LIBRARIES}
        ${PLATFORM_LIBS})
    set_property(TARGET pylabsound PROPERTY C_STANDARD 11)
    set_property(TARGET pylabsound PROPERTY CXX_STANDARD 17)
    target_include_directories(pylabsound PRIVATE
             "${LABSOUNDDEMO_ROOT}" 
             "${nanobind_SOURCE_DIR}/include"
             "{Python3_INCLUDE_DIRS}")
    install(TARGETS pylabsound LIBRARY DESTINATION bin)

    # Report a path to add to PYTHONPATH
    message(STATUS "Add ${CMAKE_CURRENT_BINARY_DIR} to your PYTHONPATH to use the LabSound python bindings")
endif()

# SAMPLES
#
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

if (APPLE)
set(SWIFT_SOURCE_FILES
    "labsound-c.h"
    "labsound-c.cpp"
    "tinycthread.h"
    "tinycthread.c"
    "flecs.h"
    "flecs.c"
    "Swift/LabSoundWrapper.h"
    "Swift/LabSoundWrapper.mm"
    "Swift/LabSoundDemoApp.swift"
    "Swift/LabSoundDemo (iOS)-Bridging-Header.h"
    "Swift/LabSoundDemo (macOS)-Bridging-Header.h"
    "Swift/Controls/Control.swift"
    "Swift/Controls/ControlGeometry.swift"
    "Swift/Controls/Helpers.swift"
    "Swift/Controls/Implementations/Ribbon.swift"
    "Swift/Controls/Implementations/SmallKnob.swift"
    "Swift/Controls/PlanarGeometry.swift"
    "Swift/Controls/SingleTouchView.swift"
)
set(COPYRIGHT "2023 Nick Porcino\nBSD License")
set(ICON_NAME "LabSoundDemo.icns")
set(ICON_PATH "assets/LabSoundDemo.icns")
set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

enable_language("Swift")

add_executable(LabSoundSwiftDemo MACOSX_BUNDLE ${SWIFT_SOURCE_FILES} ${ICON_PATH})

set_property(TARGET LabSoundSwiftDemo PROPERTY C_STANDARD 11)
set_property(TARGET LabSoundSwiftDemo PROPERTY CXX_STANDARD 17)

target_link_libraries(LabSoundSwiftDemo 
    LabSound::LabSound 
    LabSoundRtAudio::LabSoundRtAudio
    ${PLATFORM_LIBS})
target_include_directories(LabSoundSwiftDemo PRIVATE "${LABSOUNDDEMO_ROOT}")

set_target_properties(LabSoundSwiftDemo PROPERTIES
  Swift_LANGUAGE_VERSION 5.0
  XCODE_ATTRIBUTE_SWIFT_OBJC_INTERFACE_HEADER_NAME "LabSoundWrapper.h"
  XCODE_ATTRIBUTE_DERIVED_FILE_DIR "${PROJECT_BINARY_DIR}"
  XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER "${PROJECT_SOURCE_DIR}/Swift/LabSoundDemo (macOS)-Bridging-Header.h"
  MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}
  MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT}
)

install(TARGETS LabSoundSwiftDemo
    CONFIGURATIONS Debug Release
    BUNDLE DESTINATION Debug/ COMPONENT Runtime
    RUNTIME DESTINATION Debug/ COMPONENT Runtime
    BUNDLE DESTINATION Release/ COMPONENT Runtime
    RUNTIME DESTINATION Release/ COMPONENT Runtime
)

# Install dynamic libraries to the bundle
# https://github.com/ionyshch/cmake-bundle-macos/blob/master/CMakeLists.txt
set(APPS "${CMAKE_BINARY_DIR}/Debug/${PROJECT_NAME}.app")
install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\" \"${CMAKE_BINARY_DIR}/Debug\")"
)

set(CPACK_GENERATOR "DRAGNDROP")
include(CPack)

endif(APPLE)

