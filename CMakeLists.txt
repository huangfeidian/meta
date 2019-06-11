cmake_minimum_required(VERSION 3.8)
PROJECT(meta)
#dependency


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
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

set(META_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(META_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(META_TEST_DIR ${CMAKE_SOURCE_DIR}/test)

include_directories(${META_INCLUDE_DIR})

FILE(GLOB_RECURSE META_SRC_LISTS ${META_SOURCE_DIR}/*.cpp)



add_executable(type_print ${META_TEST_DIR}/type_print.cpp ${META_SRC_LISTS})
target_link_libraries(type_print PUBLIC ${LIBCLANG_LIBRARY})
target_link_libraries(type_print PRIVATE nlohmann_json::nlohmann_json)


