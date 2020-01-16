
cmake_minimum_required(VERSION 3.8)
PROJECT(meta_test)
#dependency


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)

# add json support
find_package(nlohmann_json CONFIG REQUIRED)

# add log support
find_package(spdlog CONFIG REQUIRED)
INCLUDE_DIRECTORIES(${spdlog_include_dirs})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)


add_executable(encode_test ${CMAKE_CURRENT_SOURCE_DIR}/encode/encode_test.cpp)
target_link_libraries(encode_test PUBLIC nlohmann_json::nlohmann_json)

add_executable(decode_test ${CMAKE_CURRENT_SOURCE_DIR}/decode/decode_test.cpp)
target_link_libraries(decode_test PUBLIC nlohmann_json::nlohmann_json)

add_executable(container_test ${CMAKE_CURRENT_SOURCE_DIR}/container/container.cpp)
target_link_libraries(container_test PUBLIC nlohmann_json::nlohmann_json)

add_executable(property_test ${CMAKE_CURRENT_SOURCE_DIR}/property/property_test.cpp)
target_link_libraries(property_test PUBLIC nlohmann_json::nlohmann_json)

add_executable(attr_test ${CMAKE_CURRENT_SOURCE_DIR}/attr/attr_test.cpp)
target_link_libraries(attr_test PUBLIC nlohmann_json::nlohmann_json)

add_executable(any_value_test ${CMAKE_CURRENT_SOURCE_DIR}/any_value/any_value.cpp)
target_link_libraries(any_value_test PUBLIC nlohmann_json::nlohmann_json)

add_executable(annotation_test ${CMAKE_CURRENT_SOURCE_DIR}/annotation/annotation_test.cpp)
target_link_libraries(annotation_test PRIVATE meta)

add_executable(type_print ${CMAKE_CURRENT_SOURCE_DIR}/type_print.cpp)
target_link_libraries(type_print meta)