cmake_minimum_required (VERSION 2.6)
project(cpp-test)

add_executable(test-cpp
main.cpp
)
add_definitions(-std=c++14)
target_link_libraries(test-cpp  pthread)
install(TARGETS test-cpp DESTINATION ~/test_output)

