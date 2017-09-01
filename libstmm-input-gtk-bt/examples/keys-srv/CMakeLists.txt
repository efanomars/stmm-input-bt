if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the keys-srv source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.0)

project(keys-srv CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()

# Version
set(KEYS_SRV_VERSION "0.1.0")

# Required libraries
pkg_check_modules(STMMINPUTGTKBT  REQUIRED  stmm-input-gtk-bt>=0.1.0)

list(APPEND KEYS_SRV_EXTRA_INCLUDE_DIRS   ${STMMINPUTGTKBT_INCLUDE_DIRS})
list(APPEND KEYS_SRV_EXTRA_LIBS           ${STMMINPUTGTKBT_LIBRARIES})

# Add include directories of used libraries
include_directories("${KEYS_SRV_EXTRA_INCLUDE_DIRS}")

# Source and headers files
set(KEYS_SRV_SOURCES
        ${PROJECT_SOURCE_DIR}/src/keyswindow.h
        ${PROJECT_SOURCE_DIR}/src/keyswindow.cc
        ${PROJECT_SOURCE_DIR}/src/main.cc
        )
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMM_COMPILE_WARNINGS "-Wall -Wextra -Werror") # "-Wpedantic"

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

# Define executable
add_executable(keys-srv ${KEYS_SRV_SOURCES})

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "keys-srv was configured with the following options:")
message(STATUS " KEYS_SRV_SOURCES:                ${KEYS_SRV_SOURCES}")
message(STATUS " KEYS_SRV_DATA_FILES              ${KEYS_SRV_DATA_FILES}")
message(STATUS " KEYS_SRV_EXTRA_INCLUDE_DIRS:     ${KEYS_SRV_EXTRA_INCLUDE_DIRS}")
message(STATUS " KEYS_SRV_EXTRA_LIBS:             ${KEYS_SRV_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:           ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " install prefix:                  ${CMAKE_INSTALL_PREFIX}")
endif()

target_link_libraries(keys-srv ${KEYS_SRV_EXTRA_LIBS})

#install(TARGETS keys-srv RUNTIME DESTINATION "bin")
