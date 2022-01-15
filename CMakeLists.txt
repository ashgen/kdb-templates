cmake_minimum_required(VERSION 3.14)

project(Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -march=native")

add_library(kdb-cpp INTERFACE)
find_package(benchmark REQUIRED)
target_include_directories(kdb-cpp INTERFACE release/)


include_directories(thirdparty/kx/c/c include)

add_executable(kdb-example examples/examples.cpp ${PROJECT_SOURCE_DIR}/thirdparty/kx/l64/c.o)
add_executable(kdb-pubsub examples/pubsub.cpp ${PROJECT_SOURCE_DIR}/thirdparty/kx/l64/c.o)

target_include_directories(kdb-example PRIVATE thirdparty/kx/c/c include)
target_link_libraries(kdb-example benchmark::benchmark
        "kdb-cpp"
        "pthread")
target_link_libraries(kdb-pubsub benchmark::benchmark
        "kdb-cpp"
        "pthread")
