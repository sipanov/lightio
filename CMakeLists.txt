#
# Copyright (c) Sergey Panov 2018, 2022
# Copyright 2018 Second Star Algonumerix LLC
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.4)

project(lightio)

FIND_PACKAGE(Boost 1.34
        COMPONENTS
        program_options
        REQUIRED)

add_executable(csvio_tests csvio_tests.cpp)
add_executable(octio_test octio_test.cpp octio.hpp octio_impl.hpp)
add_executable(octio_write_test octio_write_test.cpp octio.hpp octio_impl.hpp)
add_executable(ioprobe_test ioprobe_test.cpp ioprobe.hpp)

target_link_libraries(csvio_tests ${Boost_LIBRARIES})
