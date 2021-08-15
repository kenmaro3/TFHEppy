cmake_minimum_required(VERSION 2.8.12)
project(tfheppy)
set(CMAKE_CXX_STANDARD 17)

OPTION(USE_80BIT_SECURITY "Use 80bit security parameter(faster)" ON)

set(CMAKE_CXX_FLAGS "-O2 -std=c++11 -Wall -fPIC -fopenmp -fpermissive")

file(GLOB tfhe++_sources "${PROJECT_SOURCE_DIR}/thirdparties/TFHEpp/src/*.cpp")

OPTION(USE_80BIT_SECURITY "Use 80bit security parameter(faster)" ON)
if(USE_80BIT_SECURITY)
    add_compile_definitions(USE_80BIT_SECURITY)
endif()

#
#add_library(tfhe++ SHARED
#  ${tfhe++_sources}
#  )

#add_library(tfhe++
#  ${PROJECT_SOURCE_DIR}/thirdparties/TFHEpp/src/tlwe.cpp
# )


add_subdirectory(thirdparties/pybind11)
add_subdirectory(thirdparties)

pybind11_add_module(tfheppy
  src/test.cpp
  ${tfhe++_sources}
  )

target_include_directories(tfheppy
    PUBLIC ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/thirdparties/TFHEpp/include
    ${PROJECT_SOURCE_DIR}/thirdparties/TFHEpp/thirdparties/cereal/include
    ${PROJECT_SOURCE_DIR}/thirdparties/TFHEpp/thirdparties/randen
    ${PROJECT_SOURCE_DIR}/thirdparties/TFHEpp/thirdparties/spqlios
    )


target_link_libraries(tfheppy PRIVATE tfhe++ spqlios randen)
