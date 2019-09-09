cmake_minimum_required(VERSION 3.8)
PROJECT(meta)
#dependency


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)

find_package(LLVM REQUIRED)

# add LLVM includes
include_directories(${LLVM_INCLUDE_DIRS})
# add json support
find_package(nlohmann_json CONFIG REQUIRED)

# add log support
find_package(spdlog CONFIG REQUIRED)
INCLUDE_DIRECTORIES(${spdlog_include_dirs})

link_directories(${LLVM_LIBRARY_DIR})

# statically link with  LibClang and json

set(META_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/parser)
set(META_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(META_TEST_DIR ${CMAKE_SOURCE_DIR}/test)
set(META_EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/example)

include_directories(${META_INCLUDE_DIR})

FILE(GLOB_RECURSE META_SRC_LISTS ${META_SOURCE_DIR}/*.cpp)

add_library(meta ${META_SRC_LISTS})
target_link_libraries(meta PUBLIC ${LIBCLANG_LIBRARY})
target_link_libraries(meta PUBLIC nlohmann_json::nlohmann_json)

add_executable(type_print ${META_TEST_DIR}/type_print.cpp)
target_link_libraries(type_print meta)

add_executable(generate_encode ${META_EXAMPLE_DIR}/generate_encode/generate_encode.cpp)
target_link_libraries(generate_encode PRIVATE meta)

add_executable(encode_test ${META_TEST_DIR}/encode/encode_test.cpp)

add_executable(decode_test ${META_TEST_DIR}/decode/decode_test.cpp)
add_executable(container_test ${META_TEST_DIR}/container/container.cpp)
add_executable(property_test ${META_TEST_DIR}/property/property_test.cpp)
add_executable(attr_test ${META_TEST_DIR}/attr/attr_test.cpp)

add_executable(annotation_test ${META_TEST_DIR}/annotation/annotation_test.cpp)
target_link_libraries(annotation_test PRIVATE meta)

add_executable(generate_decode ${META_EXAMPLE_DIR}/generate_decode/generate_decode.cpp)
target_link_libraries(generate_decode PRIVATE meta)

add_executable(generate_property ${META_EXAMPLE_DIR}/generate_property/generate_property.cpp)
target_link_libraries(generate_property PRIVATE meta)

add_executable(generate_rpc ${META_EXAMPLE_DIR}/generate_rpc/generate_rpc.cpp)
target_link_libraries(generate_rpc PRIVATE meta)

add_executable(generate_attr ${META_EXAMPLE_DIR}/generate_attr/generate_attr.cpp)
target_link_libraries(generate_attr PRIVATE meta)

add_executable(generate_component ${META_EXAMPLE_DIR}/generate_component/generate_component.cpp)
target_link_libraries(generate_component PRIVATE meta)

add_executable(generate_enum ${META_EXAMPLE_DIR}/generate_enum/generate_enum.cpp)
target_link_libraries(generate_enum PRIVATE meta)

