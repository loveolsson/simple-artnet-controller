file (GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(${ProjectName}
    ${SOURCES}
)



find_library(libartnet_LIB NAMES libartnet.a REQUIRED HINTS ${CMAKE_SOURCE_DIR}/fakeroot/lib)
find_path(libartnet_INC artnet/artnet.h HINTS ${CMAKE_SOURCE_DIR}/fakeroot/include/)

find_library(libOSCpp_LIB NAMES libOSC++.a REQUIRED HINTS ${CMAKE_SOURCE_DIR}/fakeroot/lib)
find_path(libOSCpp_INC OSC++/OSCProcessor.h HINTS ${CMAKE_SOURCE_DIR}/fakeroot/include/)

find_library(RtMidi_LIB NAMES librtmidi.a REQUIRED HINTS ${CMAKE_SOURCE_DIR}/fakeroot/lib)
find_path(RtMidi_INC rtmidi/RtMidi.h HINTS ${CMAKE_SOURCE_DIR}/fakeroot/include/)

find_path(sole_INC sole.hpp HINTS ${CMAKE_SOURCE_DIR}/submodules/sole/)

include_directories( ${CMAKE_SOURCE_DIR}/resources/static )
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${libartnet_INC} )
include_directories( ${libOSCpp_INC} )
include_directories( ${RtMidi_INC} )
include_directories( ${sole_INC} )
include_directories( ${CMAKE_SOURCE_DIR}/submodules/json/include )
include_directories( ${CMAKE_SOURCE_DIR}/submodules/concurrentqueue )

find_package(Boost COMPONENTS system REQUIRED)

IF (APPLE)
   find_library(CoreMIDI_LIB CoreMIDI REQUIRED)
   find_library(CoreAudio_LIB CoreAudio REQUIRED)
   find_library(CoreFoundation_LIB CoreFoundation REQUIRED)
ENDIF (APPLE)

target_link_libraries(${ProjectName}
    ${libartnet_LIB}
    ${RtMidi_LIB}
    ${libOSCpp_LIB}
    ${CoreMIDI_LIB}
    ${CoreAudio_LIB}
    ${CoreFoundation_LIB}
    restinio::restinio
    Boost::system
) 