cmake_minimum_required(VERSION 3.10)

project(test_devs)

#find_package(Threads)

include_directories("./include/")

add_executable(test_devs main.cpp Clerk.h Customer.h Generator.h Observer.cpp Observer.h)
