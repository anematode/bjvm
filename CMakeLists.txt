cmake_minimum_required(VERSION 3.28)
project(browser_jvm)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

file(GLOB ZIPLIB_SOURCES vendor/ziplib/Source/ZipLib/*.cpp vendor/ziplib/Source/ZipLib/*.c)

add_custom_target(BuildZiplib ALL
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/libzip.a
    COMMAND ${CMAKE_C_COMPILER} -c -I${CMAKE_CURRENT_SOURCE_DIR}/vendor/ziplib/Source/ZipLib -O2 ${ZIPLIB_SOURCES}
    COMMAND ${CMAKE_AR} rcs libzip.a *.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(ziplib STATIC IMPORTED)

set_target_properties(
    ziplib
    PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/libzip.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ziplib/Source/ZipLib
)

add_dependencies(ziplib BuildZiplib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -fexceptions -fwasm-exceptions")
set(EmscriptenFlags "-g -s EXPORTED_FUNCTIONS=\"['_malloc','_main']\" -s TOTAL_MEMORY=1024MB")

add_library(bjvm OBJECT src/classfile.cc src/classfile.h src/constant_pool.cc src/constant_pool.h src/byte_reader.cc
        src/execution_frame.cc
        src/execution_frame.h
        src/heap_object.cc
        src/heap_object.h
        src/vm.cc
        src/vm.h
        src/bytecode_interpreter.cc
        src/bytecode_interpreter.h
        src/utilities.h
        src/utilities.cc
        src/native/string.cc
        src/native/string.h
        src/class_instance.cc
        src/class_instance.h)

# target_link_libraries(bjvm PRIVATE ziplib)

add_executable(browser_jvm src/main.cc)
target_link_libraries(browser_jvm PRIVATE bjvm)
set_target_properties(browser_jvm PROPERTIES LINK_FLAGS "${EmscriptenFlags}")

add_executable(tests test/tests.cc)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE bjvm)
set_target_properties(tests PROPERTIES LINK_FLAGS "${EmscriptenFlags}")
