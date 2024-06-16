cmake_minimum_required(VERSION 3.27)
project(UnitTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        master # or use release tag, eg. v4.0
    )
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
# Needed because of confict with libc (see more in CppUTest docs)
set(CPPUTEST_MEM_LEAK_DETECTION_DISABLED ON CACHE BOOL "Switch off CppUTest leak detection" FORCE)
FetchContent_MakeAvailable(CppUTest)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        8.1.1
)
FetchContent_MakeAvailable(fmt)

function(buildTests suiteName srcs incls)
    add_executable(${suiteName} ${srcs} ${tests})
    target_include_directories(${suiteName} PRIVATE ${incls})
    target_compile_options(${suiteName} PUBLIC -fsanitize=address -O0 -g)
    target_link_libraries(${suiteName} PRIVATE CppUTest::CppUTest CppUTest::CppUTestExt nlohmann_json::nlohmann_json fmt::fmt)
    target_compile_features(${suiteName} PRIVATE cxx_std_17)
    target_compile_definitions(${suiteName} PRIVATE UNIT_TESTS)
    target_link_options(${suiteName} PRIVATE -fsanitize=address)
endfunction()

set(INCLS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/stubs
)

set(SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MessageEncoder.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/tests/TestWebPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/tests/TestMessageEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/tests/TestHumidifierUart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/tests/TestTimer.cpp
)


buildTests(UnitTests "${SRCS}" "${INCLS}")
