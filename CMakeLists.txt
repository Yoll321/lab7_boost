cmake_minimum_required(VERSION 3.8...3.31)

set(PATCH_VERSION "1" CACHE STRING "Patch version provided by GitHub Actions")
project(lab7
    LANGUAGES CXX
    VERSION 1.0.${PATCH_VERSION}
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

set(CPACK_PACKAGE_NAME "duplicate-finder")
set(CPACK_PACKAGE_VENDOR "nom4")
set(CPACK_PACKAGE_CONTACT "nom4@tpu.ru")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Boost based c++ duplicate finding utility program")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "nom4@tpu.ru")
endif()

include(CPack)
