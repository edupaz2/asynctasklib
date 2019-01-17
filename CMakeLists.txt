cmake_minimum_required(VERSION 3.9)
project(asynctasklib VERSION 0.1 DESCRIPTION "asynctasklib description")
add_definitions(-std=c++11)
set(CXX_FLAGS -"Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
include(GNUInstallDirs)
add_library(asynctasklib SHARED src-lib/asynctasklib.cpp src-lib/processor.cpp src-lib/priority_scheduler.cpp)
set_target_properties(asynctasklib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/asynctasklib.h)
target_include_directories(asynctasklib PRIVATE src-lib include)
install(TARGETS asynctasklib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_file(asynctasklib.pc.in asynctasklib.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/asynctasklib.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)