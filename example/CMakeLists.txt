cmake_minimum_required(VERSION 3.8)
PROJECT(meta_example)
#dependency


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)






add_executable(generate_decode ${CMAKE_CURRENT_SOURCE_DIR}/generate_decode/generate_decode.cpp)
target_link_libraries(generate_decode PRIVATE meta)

add_executable(generate_property ${CMAKE_CURRENT_SOURCE_DIR}/generate_property/generate_property.cpp)
target_link_libraries(generate_property PRIVATE meta)

add_executable(generate_rpc ${CMAKE_CURRENT_SOURCE_DIR}/generate_rpc/generate_rpc.cpp)
target_link_libraries(generate_rpc PRIVATE meta)

add_executable(generate_attr ${CMAKE_CURRENT_SOURCE_DIR}/generate_attr/generate_attr.cpp)
target_link_libraries(generate_attr PRIVATE meta)

add_executable(generate_component ${CMAKE_CURRENT_SOURCE_DIR}/generate_component/generate_component.cpp)
target_link_libraries(generate_component PRIVATE meta)

add_executable(generate_enum ${CMAKE_CURRENT_SOURCE_DIR}/generate_enum/generate_enum.cpp)
target_link_libraries(generate_enum PRIVATE meta)

add_executable(generate_behavior ${CMAKE_CURRENT_SOURCE_DIR}/generate_behavior/generate_behavior.cpp)
target_link_libraries(generate_behavior PRIVATE meta)