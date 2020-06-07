cmake_minimum_required(VERSION 3.16)
project(gen_interface_v2)

function(runtime_embedded runtime_file runtime_cpp)
    file(READ ${runtime_file} content)
    set(deli "****")
    set(content "[[maybe_unused]] char runtime_text[] = R\"${deli}(\n${content})${deli}\";")
    file(WRITE ${runtime_cpp} "${content}")
endfunction(runtime_embedded)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Werror -Wall -pedantic")

runtime_embedded(src/Runtime.h src/Runtime.cpp)

file(GLOB SRC src/*.cpp)
add_executable(gen_interface_v2 main.cpp ${SRC})

add_executable(test_parser test/TestParser.cpp ${SRC})
add_executable(test_interface test/TestInterface.cpp ${SRC})
add_executable(example_rw example/example.cpp)