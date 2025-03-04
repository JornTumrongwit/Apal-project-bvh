cmake_minimum_required(VERSION 3.5)

project(RaytracingBench)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++20 -O3")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
add_executable(raytracing main.cpp matvec.cpp objects.cpp raytracer.cpp readfile.cpp Transform.cpp bbox.cpp)
target_include_directories(raytracing PRIVATE "${PROJECT_SOURCE_DIR}/libmorton/include")
