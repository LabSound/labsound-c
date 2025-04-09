
cmake_minimum_required (VERSION 3.8)
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
