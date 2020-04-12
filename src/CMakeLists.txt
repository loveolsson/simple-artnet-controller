file (GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.mm
)

add_executable(${ProjectName}
    ${SOURCES}
)

find_library(libartnet_LIB NAMES libartnet.a REQUIRED HINTS ${CMAKE_SOURCE_DIR}/fakeroot/lib)
find_path(libartnet_INC artnet/artnet.h HINTS ${CMAKE_SOURCE_DIR}/fakeroot/include/)

find_library(libOSCpp_LIB NAMES libOSC++.a REQUIRED HINTS ${CMAKE_SOURCE_DIR}/fakeroot/lib)
find_path(libOSCpp_INC OSC++/OSCProcessor.h HINTS ${CMAKE_SOURCE_DIR}/fakeroot/include/)

find_path(sole_INC sole.hpp HINTS ${CMAKE_SOURCE_DIR}/submodules/sole/)

include_directories( ${CMAKE_SOURCE_DIR}/submodules/cpp-httplib )
include_directories( ${CMAKE_SOURCE_DIR}/resources/static )
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${libartnet_INC} )
include_directories( ${libOSCpp_INC} )
include_directories( ${sole_INC} )

target_link_libraries(${ProjectName}
    nlohmann_json::nlohmann_json
    ${libartnet_LIB}
    ${libOSCpp_LIB}
) 