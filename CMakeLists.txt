cmake_minimum_required(VERSION 3.0)

link_directories(tblis-c/target/lib)
include_directories(tblis-c/target/include)

add_executable(a.out test.c)
target_link_libraries(a.out tblis)
