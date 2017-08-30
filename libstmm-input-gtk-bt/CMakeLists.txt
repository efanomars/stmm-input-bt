# File: libstmm-input-gtk-bt/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-gtk-bt CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()
DefineCommonOptions()
DefineCommonCompileOptions()

include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_GTK_BT_MAJOR_VERSION 0)
set(STMM_INPUT_GTK_BT_MINOR_VERSION 1)
set(STMM_INPUT_GTK_BT_VERSION "${STMM_INPUT_GTK_BT_MAJOR_VERSION}.${STMM_INPUT_GTK_BT_MINOR_VERSION}.0")

# required stmm-input-ev version
set(STMM_INPUT_EV_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_EV_MINOR_VERSION_REQ 1)
set(STMM_INPUT_EV_VERSION_REQ "${STMM_INPUT_EV_MAJOR_VERSION_REQ}.${STMM_INPUT_EV_MINOR_VERSION_REQ}")

# required stmm-input-ev version
set(STMM_INPUT_GTK_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_GTK_MINOR_VERSION_REQ 1)
set(STMM_INPUT_GTK_VERSION_REQ "${STMM_INPUT_GTK_MAJOR_VERSION_REQ}.${STMM_INPUT_GTK_MINOR_VERSION_REQ}")

set(BLUETOOTH_VERSION_REQ "5.23")

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(STMMINPUTEV    REQUIRED  stmm-input-ev>=${STMM_INPUT_EV_VERSION_REQ})
pkg_check_modules(STMMINPUTGTK   REQUIRED  stmm-input-gtk>=${STMM_INPUT_GTK_VERSION_REQ})
pkg_check_modules(BLUETOOTH      REQUIRED  bluez>=${BLUETOOTH_VERSION_REQ})

list(APPEND STMMI_GTK_BT_EXTRA_INCLUDE_DIRS
    ${STMMINPUTEV_INCLUDE_DIRS}
    ${STMMINPUTGTK_INCLUDE_DIRS}
    ${BLUETOOTH_INCLUDE_DIRS}
    )
list(APPEND STMMI_GTK_BT_EXTRA_LIBS
    ${STMMINPUTGTK_LIBRARIES}
    ${STMMINPUTEV_LIBRARIES}
    ${BLUETOOTH_LIBRARIES}
    )

string(STRIP "${STMMI_GTK_BT_EXTRA_LIBS}"         STMMI_GTK_BT_EXTRA_LIBS)
string(STRIP "${STMMI_GTK_BT_EXTRA_INCLUDE_DIRS}" STMMI_GTK_BT_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_GTK_BT_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-gtk-bt")

set(STMMI_HEADERS
        ${STMMI_HEADERS_DIR}/btgtkdevicemanager.h
        ${STMMI_HEADERS_DIR}/stmm-input-gtk-bt.h
        ${STMMI_HEADERS_DIR}/stmm-input-gtk-bt-config.h
        )
if (BUILD_SHARED_LIBS)
    set(STMMI_HEADERS  ${STMMI_HEADERS}
        ${STMMI_HEADERS_DIR}/libmain.h
        )
endif()
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        ${STMMI_SOURCES_DIR}/bluetoothsources.h
        ${STMMI_SOURCES_DIR}/bluetoothsources.cc
        ${STMMI_SOURCES_DIR}/btgtkbackend.h
        ${STMMI_SOURCES_DIR}/btgtkbackend.cc
        ${STMMI_SOURCES_DIR}/btgtkdevicemanager.cc
        ${STMMI_SOURCES_DIR}/btgtkkeysdevice.h
        ${STMMI_SOURCES_DIR}/btgtkkeysdevice.cc
        )
if (BUILD_SHARED_LIBS)
    set(STMMI_SOURCES  ${STMMI_SOURCES}
        ${STMMI_SOURCES_DIR}/libmain.cc
        )
endif()

# Define library
add_library(stmm-input-gtk-bt ${STMMI_SOURCES} ${PROJECT_BINARY_DIR}/stmm-input-gtk-bt-config.cc)

DefineTargetPublicCompileOptions(stmm-input-gtk-bt)

# Set version for stmm-input-gtk-bt-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_GTK_BT_VERSION}")
set(STMMI_PKG_PLUGIN_NAME "stmm-input-gtk-bt")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-gtk-bt-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-bt-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_GTK_BT_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for shared library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-gtk-bt.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-bt.pc" @ONLY)

include(GNUInstallDirs)
set(STMMI_PLUGINS_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/stmm-input-dl")
set(STMMI_PLUGINS_USER_DATA_DIR "$ENV{HOME}/.local/${CMAKE_INSTALL_DATADIR}/stmm-input-dl")

set(STMMI_GTK_BT_INSTALL_LIB_PATH1 "${CMAKE_INSTALL_PREFIX}/lib/libstmm-input-gtk-bt.so")
set(STMMI_GTK_BT_INSTALL_LIB_PATH2 "${CMAKE_INSTALL_PREFIX}/lib/libstmm-input-gtk-bt.so.${STMM_INPUT_GTK_BT_MAJOR_VERSION}")
set(STMMI_GTK_BT_INSTALL_LIB_PATH3 "libstmm-input-gtk-bt.so")
set(STMMI_GTK_BT_INSTALL_LIB_PATH4 "libstmm-input-gtk-bt-so.${STMM_INPUT_GTK_BT_MAJOR_VERSION}")
configure_file("${PROJECT_SOURCE_DIR}/${STMMI_PKG_PLUGIN_NAME}.dlp.in"
               "${PROJECT_BINARY_DIR}/${STMMI_PKG_PLUGIN_NAME}.dlp" @ONLY)

# TODO only if plugin => create option COMPILE_AS_PLUGIN=ON (default)
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") AND (BUILD_SHARED_LIBS))
    target_compile_options(stmm-input-gtk-bt PUBLIC "-Wl,-E")
endif()

message(STATUS "")
message(STATUS "stmm-input-gtk-bt was configured with the following options:")
message(STATUS " STMMI_HEADERS:                   ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                   ${STMMI_SOURCES}")
message(STATUS " STMMI_GTK_BT_EXTRA_INCLUDE_DIRS: ${STMMI_GTK_BT_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_GTK_BT_EXTRA_LIBS:         ${STMMI_GTK_BT_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                  ${STMMI_GTK_BT_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                      ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                   ${BUILD_TESTING}")
if (BUILD_SHARED_LIBS)
message(STATUS " STMMI_PLUGINS_DATA_DIR:          ${STMMI_PLUGINS_DATA_DIR}")
message(STATUS " STMMI_PLUGINS_USER_DATA_DIR:     ${STMMI_PLUGINS_USER_DATA_DIR}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-gtk-bt doc)
endif()

# Testing
add_subdirectory(googletest)
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-gtk-bt PROPERTIES
        VERSION ${STMM_INPUT_GTK_BT_VERSION}
        SOVERSION ${STMM_INPUT_GTK_BT_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-gtk-bt-${STMM_INPUT_GTK_BT_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-gtk-bt    BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-gtk-bt>  # <prefix>/include/stmm-input-gtk-bt
)

target_link_libraries(stmm-input-gtk-bt ${STMMI_GTK_BT_EXTRA_LIBS}) # ${STMMI_GTK_BT_EXTRA_LDFLAGS}

install(TARGETS stmm-input-gtk-bt LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-gtk-bt")

install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-gtk-bt.pc   DESTINATION "lib/pkgconfig")

if (BUILD_SHARED_LIBS)
    install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-gtk-bt.dlp  DESTINATION "${STMMI_PLUGINS_DATA_DIR}")
endif()

