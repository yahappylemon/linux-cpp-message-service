cmake_minimum_required(VERSION 3.16)

project(linux_cpp_thread_safe_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(thread_safe_queue
    src/thread_safe_queue.cpp
)

target_include_directories(thread_safe_queue PUBLIC include)
target_compile_options(thread_safe_queue PRIVATE -Wall -Wextra -Wpedantic)

add_executable(thread_safe_queue_demo
    src/main.cpp
)

target_link_libraries(thread_safe_queue_demo PRIVATE
    thread_safe_queue
    Threads::Threads
)
target_compile_options(thread_safe_queue_demo PRIVATE -Wall -Wextra -Wpedantic)

add_executable(thread_safe_queue_test
    tests/thread_safe_queue_test.cpp
)

target_link_libraries(thread_safe_queue_test PRIVATE
    thread_safe_queue
    Threads::Threads
)
target_compile_options(thread_safe_queue_test PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME thread_safe_queue_test COMMAND thread_safe_queue_test)
endif()
