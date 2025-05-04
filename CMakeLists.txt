cmake_minimum_required(VERSION 3.12)
project(nexus
	VERSION 0.1.0
	DESCRIPTION "nexus mew edition"
	HOMEPAGE_URL "https://github.com/raidcoregg/nexus"
	LANGUAGES CXX
)
include(GNUInstallDirs)
set(CMAKE_SHARED_LIBRARY_SUFFIX_CXX .dll)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(NEXUS_ROOT "${CMAKE_SOURCE_DIR}")
set(SRC_ROOT "${NEXUS_ROOT}/src")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
endif()

find_package(PkgConfig)

add_subdirectory(./src)
