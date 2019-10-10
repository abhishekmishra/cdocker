cmake_minimum_required ( VERSION 3.9 )
project ( clibdocker )

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