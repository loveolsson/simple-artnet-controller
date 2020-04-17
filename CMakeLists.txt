cmake_minimum_required (VERSION 3.9)

set(ProjectName simple-artnet)
project(${ProjectName})

set (CMAKE_CXX_STANDARD 17)

set(RESTINIO_FIND_DEPS OFF)
set(RESTINIO_STAND_ALONE_ASIO_DEFINES ON)

include_directories( submodules/restinio/dev/asio/include )
add_subdirectory (submodules/restinio/dev/fmt)
add_subdirectory (submodules/restinio/dev/nodejs/http_parser)
add_subdirectory (submodules/restinio/dev/restinio)
add_subdirectory (src)

set_target_properties(${ProjectName}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )