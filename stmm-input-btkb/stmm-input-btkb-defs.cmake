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

# File:   stmm-input-btkb-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_BTKB_MAJOR_VERSION 0)
set(STMM_INPUT_BTKB_MINOR_VERSION 18) # !-U-!
set(STMM_INPUT_BTKB_VERSION "${STMM_INPUT_BTKB_MAJOR_VERSION}.${STMM_INPUT_BTKB_MINOR_VERSION}.0")

set(STMM_INPUT_BTKB_REQ_GTKMM_VERSION                   "3.22.0")
set(STMM_INPUT_BTKB_REQ_BLUETOOTH_VERSION               "5.43")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    include(FindPkgConfig)
    if (NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
    endif()
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(GTKMM     REQUIRED  gtkmm-3.0>=${STMM_INPUT_BTKB_REQ_GTKMM_VERSION})
    pkg_check_modules(BLUETOOTH REQUIRED  bluez>=${STMM_INPUT_BTKB_REQ_BLUETOOTH_VERSION})
endif()

# include dirs
list(APPEND STMMINPUTBTKB_EXTRA_INCLUDE_DIRS  "${GTKMM_INCLUDE_DIRS}")
list(APPEND STMMINPUTBTKB_EXTRA_INCLUDE_DIRS  "${BLUETOOTH_INCLUDE_DIRS}")

# libs
list(APPEND STMMINPUTBTKB_EXTRA_LIBRARIES     "${GTKMM_LIBRARIES}")
list(APPEND STMMINPUTBTKB_EXTRA_LIBRARIES     "${BLUETOOTH_LIBRARIES}")
