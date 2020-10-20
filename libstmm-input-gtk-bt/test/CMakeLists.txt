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

# File:   libstmm-input-gtk-bt/test/CMakeLists.txt

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)

    # Tests dir
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")

    # Test sources should end with .cxx, helper sources with .h .cc
    set(STMMI_GTK_BT_TEST_SOURCES
            "${STMMI_TEST_SOURCES_DIR}/testBtGtkDeviceManager.cxx"
            )

    set(STMMI_GTK_BT_TEST_WITH_SOURCES
            "${STMMI_TEST_SOURCES_DIR}/fakebtgtkbackend.h"
            "${STMMI_TEST_SOURCES_DIR}/fakebtgtkbackend.cc"
            "${STMMI_TEST_SOURCES_DIR}/fakebtgtkdevicemanager.h"
            "${STMMI_TEST_SOURCES_DIR}/fakebtgtkwindowdata.h"
            "${STMMI_TEST_SOURCES_DIR}/fixtureBtDM.h"
            "${STMMI_TEST_SOURCES_DIR}/fixtureGlibApp.h"
            "${STMMI_TEST_SOURCES_DIR}/fixtureTestBase.h"
            "${STMMI_TEST_SOURCES_DIR}/fixturevariantEventClasses.h"
            )

    TestFiles("${STMMI_GTK_BT_TEST_SOURCES}"
              "${STMMI_SOURCES};${STMMI_GTK_BT_TEST_WITH_SOURCES}"
              "${STMMINPUTGTKBT_EXTRA_INCLUDE_DIRS}" "${STMMINPUTGTKBT_EXTRA_LIBRARIES}" TRUE)

    include(CTest)
endif()
