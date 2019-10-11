cmake_minimum_required ( VERSION 3.9 )

project ( clibdocker C )

# Set the project source dir (just convention)
set( CLIBDOCKER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( CLIBDOCKER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# It's useful to setup the current version of our code in the build system
# using a `semver` style
set ( CLIBDOCKER_VERSION_MAJOR 1 )
set ( CLIBDOCKER_VERSION_MINOR 0 )
set ( CLIBDOCKER_VERSION_PATCH 0 )

# Include Directories
# In GCC, this will invoke the "-I" command
include_directories( include )

# Lists
# Setup the list of source files
set( CLIBDOCKER_SOURCES 
  src/docker_connection_util.c
  src/docker_containers.c
  src/docker_images.c
  src/docker_log.c
  src/docker_networks.c
  src/docker_result.c
  src/docker_system.c
  src/docker_util.c
  src/docker_volumes.c
  src/tinydir.h
)

add_library ( ${PROJECT_NAME} ${CLIBDOCKER_SOURCES} )

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

find_package(json-c CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE json-c::json-c)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

#find_package(libarchive CONFIG REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE libarchive::libarchive)

# To find and use azure-storage-cpp
find_path(LIBARCHIVE_INCLUDE_DIR archive.h)
find_library(LIBARCHIVE_LIBRARY archive)
include_directories(${LIBARCHIVE_INCLUDE_DIR})
link_libraries(${LIBARCHIVE_LIBRARY})
