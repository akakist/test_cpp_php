cmake_minimum_required (VERSION 2.6)

add_executable(test
main.cpp
)
add_definitions(-std=c++14)
#target_link_libraries(mt-main-${PORT}.exe megatron-main  megatron-core z  pthread ${Qt5Widgets_LIBRARIES})
install(TARGETS test DESTINATION ~/test_output)

