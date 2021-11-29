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

add_library(${CMAKE_PROJECT_NAME} ${META_SRC_LISTS})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIBCLANG_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

foreach(p LIB BIN INCLUDE)
	set(var CMAKE_INSTALL_${p}DIR)
	if(NOT IS_ABSOLUTE "${${var}}")
		string(TOLOWER ${p} lower_p)
        set(${var} "${CMAKE_INSTALL_PREFIX}/${lower_p}")
	endif()
endforeach()

export(TARGETS ${CMAKE_PROJECT_NAME}
     FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)

file(WRITE
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)\n")

install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
   ARCHIVE  DESTINATION lib
   LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
  )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include/)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

add_subdirectory(test)

add_subdirectory(example)





