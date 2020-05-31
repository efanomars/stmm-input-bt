# File: libstmm-input-gtk-bt/stmm-input-gtk-bt-defs.cmake

#  Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, see <http://www.gnu.org/licenses/>

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_GTK_BT_MAJOR_VERSION 0)
set(STMM_INPUT_GTK_BT_MINOR_VERSION 16) # !-U-!
set(STMM_INPUT_GTK_BT_VERSION "${STMM_INPUT_GTK_BT_MAJOR_VERSION}.${STMM_INPUT_GTK_BT_MINOR_VERSION}.0")

# required stmm-input-gtk version
set(STMM_INPUT_GTK_BT_REQ_STMM_INPUT_GTK_MAJOR_VERSION "0")
set(STMM_INPUT_GTK_BT_REQ_STMM_INPUT_GTK_MINOR_VERSION "13") # !-U-!
set(STMM_INPUT_GTK_BT_REQ_STMM_INPUT_GTK_VERSION "${STMM_INPUT_GTK_BT_REQ_STMM_INPUT_GTK_MAJOR_VERSION}.${STMM_INPUT_GTK_BT_REQ_STMM_INPUT_GTK_MINOR_VERSION}")

# required stmm-input-ev version
set(STMM_INPUT_GTK_BT_REQ_STMM_INPUT_EV_MAJOR_VERSION "0")
set(STMM_INPUT_GTK_BT_REQ_STMM_INPUT_EV_MINOR_VERSION "13") # !-U-!
set(STMM_INPUT_GTK_BT_REQ_STMM_INPUT_EV_VERSION "${STMM_INPUT_GTK_BT_REQ_STMM_INPUT_EV_MAJOR_VERSION}.${STMM_INPUT_GTK_BT_REQ_STMM_INPUT_EV_MINOR_VERSION}")

# required bluez version
set(STMM_INPUT_GTK_BT_REQ_BLUETOOTH_VERSION "5.23")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    include(FindPkgConfig)
    if (NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
    endif()
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(STMMINPUTEV    REQUIRED  stmm-input-ev>=${STMM_INPUT_GTK_BT_REQ_STMM_INPUT_EV_VERSION})
    pkg_check_modules(STMMINPUTGTK   REQUIRED  stmm-input-gtk>=${STMM_INPUT_GTK_BT_REQ_STMM_INPUT_GTK_VERSION})
    pkg_check_modules(BLUETOOTH      REQUIRED  bluez>=${STMM_INPUT_GTK_BT_REQ_BLUETOOTH_VERSION})
endif()

# include dirs
list(APPEND STMMINPUTGTKBT_EXTRA_INCLUDE_DIRS  "${STMMINPUTGTK_INCLUDE_DIRS}")
list(APPEND STMMINPUTGTKBT_EXTRA_INCLUDE_DIRS  "${STMMINPUTEV_INCLUDE_DIRS}")
list(APPEND STMMINPUTGTKBT_EXTRA_INCLUDE_DIRS  "${BLUETOOOTH_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-gtk-bt/include")

list(APPEND STMMINPUTGTKBT_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUTGTKBT_INCLUDE_DIRS  "${STMMINPUTGTKBT_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES    "")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES     "${STMMINPUTGTK_LIBRARIES}")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES     "${STMMINPUTEV_LIBRARIES}")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES     "${BLUETOOTH_LIBRARIES}")

set(        STMMINPUTGTKBT_EXTRA_LIBRARIES     "")
list(APPEND STMMINPUTGTKBT_EXTRA_LIBRARIES     "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-gtk-bt/build/libstmm-input-gtk-bt.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-gtk-bt/build/libstmm-input-gtk-bt.a")
endif()

list(APPEND STMMINPUTGTKBT_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUTGTKBT_LIBRARIES "${STMMINPUTGTKBT_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-input-gtk-bt  ${STMMI_LIB_FILE}  "${STMMINPUTGTKBT_INCLUDE_DIRS}"  "" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()

# if (("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "") AND NOT TARGET stmm-input-gtk-bt)
#     if (BUILD_SHARED_LIBS)
#         add_library(stmm-input-gtk-bt SHARED IMPORTED)
#     else()
#         add_library(stmm-input-gtk-bt STATIC IMPORTED)
#     endif()
#     set_target_properties(stmm-input-gtk-bt PROPERTIES IMPORTED_LOCATION             "${STMMI_LIB_FILE}")
#     set_target_properties(stmm-input-gtk-bt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STMMINPUTGTKBT_INCLUDE_DIRS}")
#     set_target_properties(stmm-input-gtk-bt PROPERTIES INTERFACE_LINK_LIBRARIES      "${STMMINPUTGTKBT_EXTRA_LIBRARIES}")
# endif()
