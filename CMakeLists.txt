cmake_minimum_required(VERSION 3.8)
PROJECT(meta)
#dependency


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
find_package(LLVM REQUIRED)

# add LLVM includes
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIR})
message(STATUS "libclang lib is " ${LIBCLANG_LIBRARY})
else()
find_package(Clang REQUIRED)
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
link_directories(${CLANG_LIBRARY_DIR})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})
set(LIBCLANG_LIBRARY libclang)
endif(MSVC)






include_directories(${CMAKE_SOURCE_DIR}/third_party/Mustache)

# add json support
find_package(nlohmann_json CONFIG REQUIRED)

# add log support
find_package(spdlog CONFIG REQUIRED)
INCLUDE_DIRECTORIES(${spdlog_include_dirs})




# statically link with  LibClang and json

set(META_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/parser)
set(META_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(META_TEST_DIR ${CMAKE_SOURCE_DIR}/test)
set(META_EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/example)

include_directories(${META_INCLUDE_DIR})
include_directories(${META_ANY_INCLUDE_DIR})

FILE(GLOB_RECURSE META_SRC_LISTS ${META_SOURCE_DIR}/*.cpp)

add_library(meta ${META_SRC_LISTS})
target_link_libraries(meta PUBLIC ${LIBCLANG_LIBRARY})
target_link_libraries(meta PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(test)

add_subdirectory(example)





