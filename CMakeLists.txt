cmake_minimum_required ( VERSION 3.9 )

project ( clibdocker C )

# Export all symbols for shared linking to work on windows
# see https://stackoverflow.com/questions/37128555/getting-cmake-to-build-shared-library-for-msvc
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

# Set the project source dir (just convention)
set( CLIBDOCKER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( CLIBDOCKER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# It's useful to setup the current version of our code in the build system
# using a `semver` style
set ( CLIBDOCKER_VERSION_MAJOR 1 )
set ( CLIBDOCKER_VERSION_MINOR 0 )
set ( CLIBDOCKER_VERSION_PATCH 0 )

# TODO: remove shared lib for lua when all builds move to shared lib builds
# Set the lualib name
set ( LUA_CLIBDOCKER lua${PROJECT_NAME} )

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
  src/docker_ignore.c
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
  include/docker_ignore.h
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
  test/test_docker_ignore.c
  test/test_docker_ignore.h
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND LUA_FROM_PKGCONFIG)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(lua REQUIRED IMPORTED_TARGET lua)
endif()

option (ENABLE_LUA "Enable LUA bindings for clibdocker" OFF)
if (ENABLE_LUA)
  include(FindLua)
  if (LUA_FOUND)
    message("-- lua found at ${LUA_LIBRARIES} ${LUA_INCLUDE_DIR}")
    message("-- lua version is ${LUA_VERSION_STRING}")
    add_compile_definitions(LUA_ENABLED)
    set( CLIBDOCKER_SOURCES ${CLIBDOCKER_SOURCES}
      include/lua_docker.h
      src/lua_docker.c
    )
  endif (LUA_FOUND)
endif (ENABLE_LUA)

add_library ( ${PROJECT_NAME} ${CLIBDOCKER_SOURCES} )
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_library ( ${LUA_CLIBDOCKER} SHARED ${CLIBDOCKER_SOURCES} )
set_property(TARGET ${LUA_CLIBDOCKER} PROPERTY C_STANDARD 11)
target_include_directories(${LUA_CLIBDOCKER} PUBLIC include)

if (ENABLE_LUA)
  if (LUA_FOUND)
  target_include_directories(${PROJECT_NAME} PUBLIC ${LUA_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${LUA_LIBRARIES})    
  target_include_directories(${LUA_CLIBDOCKER} PUBLIC ${LUA_INCLUDE_DIR})
  target_link_libraries(${LUA_CLIBDOCKER} PUBLIC ${LUA_LIBRARIES})    
  endif (LUA_FOUND)
endif (ENABLE_LUA)

find_package(json-c CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)
target_link_libraries(${LUA_CLIBDOCKER} PUBLIC json-c::json-c)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)
target_link_libraries(${LUA_CLIBDOCKER} PUBLIC CURL::libcurl)

# To find and use libarchive, as the vcpkg build does not have cmake config
# See https://github.com/microsoft/vcpkg/issues/8839#issuecomment-558066466
# for additional lookup to ZLIB
find_package(ZLIB REQUIRED)
find_path(LIBARCHIVE_INCLUDE_DIR archive.h)
find_library(LIBARCHIVE_LIBRARY archive)
include_directories(${LIBARCHIVE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBARCHIVE_LIBRARY} ZLIB::ZLIB)
target_link_libraries(${LUA_CLIBDOCKER} PUBLIC ${LIBARCHIVE_LIBRARY} ZLIB::ZLIB)

# set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)
target_link_libraries(${LUA_CLIBDOCKER} PUBLIC coll::coll)

option(ENABLE_TESTS "Perform unit tests after build" OFF)
set ( CLIBDOCKER_TEST_PROGRAM_NAME clibdocker_test )
if (ENABLE_TESTS)
  add_executable(${CLIBDOCKER_TEST_PROGRAM_NAME} ${CLIBDOCKER_TEST_SOURCES})
  set_property(TARGET ${CLIBDOCKER_TEST_PROGRAM_NAME} PROPERTY C_STANDARD 11)
  include_directories( test )

	# cmocka
	find_package(cmocka CONFIG REQUIRED)
	target_link_libraries(${CLIBDOCKER_TEST_PROGRAM_NAME} PRIVATE cmocka::cmocka)

  target_link_libraries(${CLIBDOCKER_TEST_PROGRAM_NAME} PRIVATE ${PROJECT_NAME})
  add_test( all_docker_tests ${CLIBDOCKER_TEST_PROGRAM_NAME} )
  enable_testing()
endif(ENABLE_TESTS)

configure_file("lua/json.lua" "json.lua" COPYONLY)

# Package Configuration
export(TARGETS clibdocker NAMESPACE clibdocker:: FILE clibdockerConfig.cmake)
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE clibdocker)
