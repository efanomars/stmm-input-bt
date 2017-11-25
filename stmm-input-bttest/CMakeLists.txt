# File: stmm-input-bttest/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-bttest CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineCommonCompileOptions()

include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()

#
set(STMM_INPUT_BTTEST_MAJOR_VERSION 0)
set(STMM_INPUT_BTTEST_MINOR_VERSION 1)
set(STMM_INPUT_BTTEST_VERSION "${STMM_INPUT_BTTEST_MAJOR_VERSION}.${STMM_INPUT_BTTEST_MINOR_VERSION}.0")

set(GTKMM_VERSION_REQ "3.14.0")
set(BLUETOOTH_VERSION_REQ "5.23")
set(GCONFMM_VERSION_REQ "2.28")

pkg_check_modules(GTKMM       REQUIRED  gtkmm-3.0>=${GTKMM_VERSION_REQ})
pkg_check_modules(BLUETOOTH   REQUIRED  bluez>=${BLUETOOTH_VERSION_REQ})
pkg_check_modules(GCONFMM     REQUIRED  gconfmm-2.6>=${GCONFMM_VERSION_REQ})

list(APPEND STMMI_BTTEST_EXTRA_INCLUDE_DIRS   ${GTKMM_INCLUDE_DIRS} ${BLUETOOTH_INCLUDE_DIRS} ${GCONFMM_INCLUDE_DIRS})
list(APPEND STMMI_BTTEST_EXTRA_LIBS           ${GTKMM_LIBRARIES}    ${BLUETOOTH_LIBRARIES}    ${GCONFMM_LIBRARIES})

string(STRIP "${STMMI_BTTEST_EXTRA_LIBS}"         STMMI_BTTEST_EXTRA_LIBS)
string(STRIP "${STMMI_BTTEST_EXTRA_INCLUDE_DIRS}" STMMI_BTTEST_EXTRA_INCLUDE_DIRS)

# Source files (and headers only used for building)
set(STMMI_BTTEST_SOURCES
        ${PROJECT_SOURCE_DIR}/src/main.cc
        #
        ${PROJECT_SOURCE_DIR}/src/btclientsources.h
        ${PROJECT_SOURCE_DIR}/src/btclientsources.cc
        ${PROJECT_SOURCE_DIR}/src/btkeyclient.h
        ${PROJECT_SOURCE_DIR}/src/btkeyclient.cc
        ${PROJECT_SOURCE_DIR}/src/btkeyservers.h
        ${PROJECT_SOURCE_DIR}/src/btkeyservers.cc
        ${PROJECT_SOURCE_DIR}/src/bttestwindow.h
        ${PROJECT_SOURCE_DIR}/src/bttestwindow.cc
        ${PROJECT_SOURCE_DIR}/src/circularbuffer.h
        ${PROJECT_SOURCE_DIR}/src/config.h
        ${PROJECT_SOURCE_DIR}/src/inputstrings.h
        ${PROJECT_SOURCE_DIR}/src/inputstrings.cc
        ${PROJECT_SOURCE_DIR}/src/keyboarddialog.h
        ${PROJECT_SOURCE_DIR}/src/keyboarddialog.cc
        ${PROJECT_SOURCE_DIR}/src/keydialog.h
        ${PROJECT_SOURCE_DIR}/src/keydialog.cc
        ${PROJECT_SOURCE_DIR}/src/keypacket.h
        ${PROJECT_SOURCE_DIR}/src/main.cc
        ${PROJECT_SOURCE_DIR}/src/weightdialog.h
        ${PROJECT_SOURCE_DIR}/src/weightdialog.cc
        )
set(STMMI_BTTEST_DATA_FILES
#         ${PROJECT_SOURCE_DIR}/data/buoy_32313.png
        )

add_executable(stmm-input-bttest ${STMMI_BTTEST_SOURCES} ${PROJECT_BINARY_DIR}/config.cc)

target_include_directories(stmm-input-bttest SYSTEM PUBLIC ${STMMI_BTTEST_EXTRA_INCLUDE_DIRS})

DefineTargetPublicCompileOptions(stmm-input-bttest)

include(GNUInstallDirs)
set(STMMI_BTTEST_PKG_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/stmm-input-bttest")
set(STMMI_BTTEST_PKG_REL_DATA_DIR "${CMAKE_INSTALL_DATADIR}/stmm-input-bttest")

# Create config file for executable
configure_file("${PROJECT_SOURCE_DIR}/src/config.cc.in"
               "${PROJECT_BINARY_DIR}/config.cc" @ONLY)
# This allows config.cc to find the config.h include
target_include_directories(stmm-input-bttest PUBLIC ${PROJECT_SOURCE_DIR}/src)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-bttest was configured with the following options:")
message(STATUS " STMMI_BTTEST_SOURCES:            ${STMMI_BTTEST_SOURCES}")
#message(STATUS " STMMI_BTTEST_EXTRA_INCLUDE_DIRS: ${STMMI_BTTEST_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_BTTEST_EXTRA_LIBS:         ${STMMI_BTTEST_EXTRA_LIBS}")
message(STATUS " STMMI_BTTEST_DATA_FILES          ${STMMI_BTTEST_DATA_FILES}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
#message(STATUS " CMAKE_CXX_FLAGS:                 $<TARGET_PROPERTY:stmm-input-bttest,COMPILE_OPTIONS>")
message(STATUS " install prefix:                  ${CMAKE_INSTALL_PREFIX}")
endif()

target_link_libraries(stmm-input-bttest ${STMMI_BTTEST_EXTRA_LIBS} -lpthread) # ${STMMI_BTTEST_EXTRA_LDFLAGS}

install(TARGETS stmm-input-bttest RUNTIME DESTINATION "bin")

install(FILES ${STMMI_BTTEST_DATA_FILES}  DESTINATION   ${STMMI_BTTEST_PKG_REL_DATA_DIR})

