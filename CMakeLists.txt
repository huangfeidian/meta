cmake_minimum_required(VERSION 3.8)
PROJECT(meta)
#dependency


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


link_directories(${LLVM_LIBRARY_DIR})

# statically link with  LibClang and json
target_link_libraries(MetaParser ${LIBCLANG_LIBRARY})
