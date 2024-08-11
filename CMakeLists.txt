cmake_minimum_required(VERSION 3.28)
project(browser_jvm)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")
set(EmscriptenFlags "-g -fwasm-exceptions -s EXPORTED_FUNCTIONS=\"['_malloc','_main']\"")

add_library(bjvm OBJECT src/classfile.cc src/classfile.h src/constant_pool.cc src/constant_pool.h src/byte_reader.cc
        src/execution_frame.cc
        src/execution_frame.h
        src/heap_object.cc
        src/heap_object.h
        src/vm.cc
        src/vm.h
        src/bytecode_interpreter.cc
        src/bytecode_interpreter.h)

add_executable(browser_jvm src/main.cc)
target_link_libraries(browser_jvm PRIVATE bjvm)
set_target_properties(browser_jvm PROPERTIES LINK_FLAGS "${EmscriptenFlags}")

add_executable(tests test/tests.cc)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE bjvm)
set_target_properties(tests PROPERTIES LINK_FLAGS "${EmscriptenFlags}")
