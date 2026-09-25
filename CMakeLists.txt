cmake_minimum_required(VERSION 3.8...3.31)
project(lab7
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS program_options filesystem system)

add_executable(lab7 
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/view.cpp
)

target_include_directories(lab7
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lab7 
    PRIVATE
        Boost::headers
        Boost::program_options
        Boost::filesystem
        Boost::system
)

set_target_properties(lab7
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)