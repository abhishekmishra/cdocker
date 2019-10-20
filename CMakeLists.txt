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

  include/docker_all.h
  include/docker_common.h
  include/docker_connection_util.h
  include/docker_containers.h
  include/docker_images.h
  include/docker_log.h
  include/docker_networks.h
  include/docker_result.h
  include/docker_system.h
  include/docker_util.h
  include/docker_volumes.h
)

set( CLIBDOCKER_TEST_SOURCES 
  test/test_all.c
  test/test_docker_containers.c
  test/test_docker_containers.h
  test/test_docker_images.c
  test/test_docker_images.h
  test/test_docker_networks.c
  test/test_docker_networks.h
  test/test_docker_system.c
  test/test_docker_system.h
  test/test_docker_volumes.c
  test/test_docker_volumes.h
  test/test_util.c
  test/test_util.h
)


add_library ( ${PROJECT_NAME} ${CLIBDOCKER_SOURCES} )
target_include_directories(${PROJECT_NAME} PRIVATE ${YOUR_DIRECTORY})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

find_package(json-c CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)

# To find and use libarchive, as the vcpkg build does not have cmake config
find_path(LIBARCHIVE_INCLUDE_DIR archive.h)
find_library(LIBARCHIVE_LIBRARY archive)
include_directories(${LIBARCHIVE_INCLUDE_DIR})
link_libraries(${LIBARCHIVE_LIBRARY})

option(ENABLE_TESTS "Perform unit tests after build" OFF)
set ( CLIBDOCKER_TEST_PROGRAM_NAME clibdocker_test )
if (ENABLE_TESTS)
  add_executable(${CLIBDOCKER_TEST_PROGRAM_NAME} ${CLIBDOCKER_TEST_SOURCES})
  set_property(TARGET ${CLIBDOCKER_TEST_PROGRAM_NAME} PROPERTY C_STANDARD 11)
  include_directories( test )

  # cmocka
  find_package(cmocka CONFIG REQUIRED)
  target_include_directories(${CLIBDOCKER_TEST_PROGRAM_NAME} PRIVATE ${CMOCKA_INCLUDE_DIR})
  target_link_libraries(${CLIBDOCKER_TEST_PROGRAM_NAME} PRIVATE ${CMOCKA_LIBRARIES})

  find_package(json-c CONFIG REQUIRED)
  target_link_libraries(${CLIBDOCKER_TEST_PROGRAM_NAME} PUBLIC json-c::json-c)

  find_package(CURL REQUIRED)
  target_link_libraries(${CLIBDOCKER_TEST_PROGRAM_NAME} PUBLIC CURL::libcurl)

  # To find and use libarchive, as the vcpkg build does not have cmake config
  find_path(LIBARCHIVE_INCLUDE_DIR archive.h)
  find_library(LIBARCHIVE_LIBRARY archive)
  include_directories(${LIBARCHIVE_INCLUDE_DIR})
  link_libraries(${LIBARCHIVE_LIBRARY})

  target_link_libraries(${CLIBDOCKER_TEST_PROGRAM_NAME} PUBLIC ${PROJECT_NAME})
  add_test( all_docker_tests ${CLIBDOCKER_TEST_PROGRAM_NAME} )
  enable_testing()
endif(ENABLE_TESTS)