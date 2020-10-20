# Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, see <http://www.gnu.org/licenses/>

# File:   stmm-input-btkb/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

option(STMM_INSTALL_MAN_PAGE "Install man page in debian.orig/" ON)
option(STMM_INSTALL_LAUNCHER "Install launcher in share/applications/ (implies STMM_INSTALL_ICONS=ON)" ON)
option(STMM_INSTALL_ICONS "Install icons in share/icons/hicolor/(size)/apps/" ON)
# The following is needed because GSettings doesn't check /usr/local (just /usr)
set(STMM_GSETTINGS_SCHEMA_INSTALL_PREFIX "/usr" CACHE PATH "GSettings schema installation prefix (absolute path)")


project(stmm-input-btkb CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake")

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineCommonCompileOptions("c++14")

# Source files (and headers only used for building)
set(STMMI_BTKB_SOURCES
        "${PROJECT_SOURCE_DIR}/src/addrscreen.h"
        "${PROJECT_SOURCE_DIR}/src/addrscreen.cc"
        "${PROJECT_SOURCE_DIR}/src/btclientsources.h"
        "${PROJECT_SOURCE_DIR}/src/btclientsources.cc"
        "${PROJECT_SOURCE_DIR}/src/btkeyclient.h"
        "${PROJECT_SOURCE_DIR}/src/btkeyclient.cc"
        "${PROJECT_SOURCE_DIR}/src/btkeyservers.h"
        "${PROJECT_SOURCE_DIR}/src/btkeyservers.cc"
        "${PROJECT_SOURCE_DIR}/src/btkbwindow.h"
        "${PROJECT_SOURCE_DIR}/src/btkbwindow.cc"
        "${PROJECT_SOURCE_DIR}/src/circularbuffer.h"
        "${PROJECT_SOURCE_DIR}/src/circularbuffer.cc"
        "${PROJECT_SOURCE_DIR}/src/config.h"
        "${PROJECT_SOURCE_DIR}/src/gtkutilpriv.h"
        "${PROJECT_SOURCE_DIR}/src/gtkutilpriv.cc"
        "${PROJECT_SOURCE_DIR}/src/hardwarekey.h"
        "${PROJECT_SOURCE_DIR}/src/hardwarekey.cc"
        "${PROJECT_SOURCE_DIR}/src/inputstrings.h"
        "${PROJECT_SOURCE_DIR}/src/inputstrings.cc"
        "${PROJECT_SOURCE_DIR}/src/keyboardscreen.h"
        "${PROJECT_SOURCE_DIR}/src/keyboardscreen.cc"
        "${PROJECT_SOURCE_DIR}/src/keypacket.h"
        "${PROJECT_SOURCE_DIR}/src/keypacket.cc"
        "${PROJECT_SOURCE_DIR}/src/keyscreen.h"
        "${PROJECT_SOURCE_DIR}/src/keyscreen.cc"
        "${PROJECT_SOURCE_DIR}/src/main.cc"
        "${PROJECT_SOURCE_DIR}/src/util.h"
        "${PROJECT_SOURCE_DIR}/src/util.cc"
        "${PROJECT_SOURCE_DIR}/src/weightscreen.h"
        "${PROJECT_SOURCE_DIR}/src/weightscreen.cc"
        )
set(STMMI_BTKB_DATA_DIR  "${PROJECT_SOURCE_DIR}/data")
set(STMMI_BTKB_DATA_FILES
#        ${STMMI_BTKB_DATA_DIR}/x.png
        )

add_executable(stmm-input-btkb ${STMMI_BTKB_SOURCES} "${PROJECT_BINARY_DIR}/config.cc")

include("stmm-input-btkb-defs.cmake")

target_include_directories(stmm-input-btkb SYSTEM PUBLIC ${STMMINPUTBTKB_EXTRA_INCLUDE_DIRS})
# This allows config.cc to find the config.h include
target_include_directories(stmm-input-btkb        PUBLIC "${PROJECT_SOURCE_DIR}/src")

target_link_libraries(stmm-input-btkb ${STMMINPUTBTKB_EXTRA_LIBRARIES})
target_link_libraries(stmm-input-btkb -lpthread)

DefineTargetPublicCompileOptions(stmm-input-btkb)

include(GNUInstallDirs)
set(STMMI_BTKB_PKG_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/stmm-input-btkb")
set(STMMI_BTKB_PKG_REL_DATA_DIR  "${CMAKE_INSTALL_DATADIR}/stmm-input-btkb")

# Create config file for executable
configure_file("${PROJECT_SOURCE_DIR}/src/config.cc.in"
               "${PROJECT_BINARY_DIR}/config.cc" @ONLY)
if (STMM_INSTALL_MAN_PAGE)
    configure_file("${PROJECT_SOURCE_DIR}/../scripts/priv/debian.orig/stmm-input-btkb.1"
                   "${PROJECT_BINARY_DIR}/stmm-input-btkb.1" @ONLY)
    CreateManGz(stmm-input-btkb "stmm-input-btkb.1")
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-btkb was configured with the following options:")
message(STATUS " STMMI_BTKB_SOURCES:               ${STMMI_BTKB_SOURCES}")
message(STATUS " STMMINPUTBTKB_EXTRA_INCLUDE_DIRS: ${STMMINPUTBTKB_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTBTKB_EXTRA_LIBRARIES:    ${STMMINPUTBTKB_EXTRA_LIBRARIES}")
message(STATUS " STMMI_BTKB_DATA_FILES             ${STMMI_BTKB_DATA_FILES}")
message(STATUS " CMAKE_BUILD_TYPE:                 ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:            ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:       ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " install prefix:                   ${CMAKE_INSTALL_PREFIX}")
endif()

install(TARGETS stmm-input-btkb RUNTIME DESTINATION "bin")

install(FILES ${STMMI_BTKB_DATA_FILES}  DESTINATION   ${STMMI_BTKB_PKG_REL_DATA_DIR})

if (STMM_INSTALL_LAUNCHER)
    install(FILES          "${STMMI_BTKB_DATA_DIR}/applications/com.efanomars.stmm-input-btkb.desktop"
            DESTINATION  "${CMAKE_INSTALL_DATADIR}/applications/")
endif()
if (STMM_INSTALL_ICONS OR STMM_INSTALL_LAUNCHER)
    install(FILES            "${STMMI_BTKB_DATA_DIR}/icons/hicolor/24x24/apps/stmm-input-btkb.png"
            DESTINATION    "${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps/")
    install(FILES            "${STMMI_BTKB_DATA_DIR}/icons/hicolor/32x32/apps/stmm-input-btkb.png"
            DESTINATION    "${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps/")
    install(FILES            "${STMMI_BTKB_DATA_DIR}/icons/hicolor/48x48/apps/stmm-input-btkb.png"
            DESTINATION    "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps/")
    install(FILES         "${STMMI_BTKB_DATA_DIR}/icons/hicolor/scalable/apps/stmm-input-btkb.svg"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps/")
endif()

if (STMM_INSTALL_MAN_PAGE)
    install(FILES                                      "${PROJECT_BINARY_DIR}/stmm-input-btkb.1.gz"
            DESTINATION                    "${CMAKE_INSTALL_DATADIR}/man/man1/")
endif()

install(FILES          "${STMMI_BTKB_DATA_DIR}/glib-2.0/schemas/com.efanomars.stmm-input-btkb.gschema.xml"
        DESTINATION  "${STMM_GSETTINGS_SCHEMA_INSTALL_PREFIX}/share/glib-2.0/schemas/")
