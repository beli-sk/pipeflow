cmake_minimum_required (VERSION 2.6)

set(PACKAGE_VERSION_MAJOR "0")
set(PACKAGE_VERSION_MINOR "0")
set(PACKAGE_VERSION_PATCH "1")
set(VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")

# set build type, accept -D CMAKE_BUILD_TYPE on command line
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "type of build (None Debug Release RelWithDebInfo MinSizeRel)")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "type of build (None Debug Release RelWithDebInfo MinSizeRel)")
ENDIF()

# mark with SVN revision if building for debug
find_package(Subversion)
if(Subversion_FOUND AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} ER)
	set(SUBVERSION_REVISION ${ER_WC_REVISION})
	set(VERSION "${VERSION}-debug${SUBVERSION_REVISION}")
endif()

project (pipeflow)

# compiler flags
if (CMAKE_COMPILER_IS_GNUCC)
	add_definitions(-Wall -pedantic -std=c99)
endif ()

add_subdirectory(src)

set(DOC_FILES LICENSE README.md)
set(DOC_PATH "share/doc/pipeflow-${VERSION}")
install(FILES ${DOC_FILES}
        DESTINATION ${DOC_PATH})

# config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# for building deb packages
SET(CPACK_PACKAGE_NAME "pipeflow")
SET(CPACK_PACKAGE_VENDOR "Beli <devel@beli.sk>")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tool that measures the flow of data through unix pipe.")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_VERSION_MAJOR "${PACKAGE_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PACKAGE_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PACKAGE_VERSION_PATCH}")
SET(CPACK_DEBIAN_PACKAGE_SECTION "util")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64") # FIXME change this!
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Beli <devel@beli.sk>")

INCLUDE(CPack)
