# stmm-input-gtk-bt: test/CMakeLists.txt

# See   github.com/Barthelemy/CppProjectTemplate

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)
    # Test dirs
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources should end with .cxx
    set(STMMI_TEST_SOURCES
            #${STMMI_TEST_SOURCES_DIR}/testPluginsDeviceManager.cxx
           )

    set(STMMI_GTK_BT_TEST_SOURCES
            ${STMMI_TEST_SOURCES_DIR}/testBtGtkDeviceManager.cxx
            )

    set(STMMI_GTK_BT_TEST_WITH_SOURCES
            ${STMMI_TEST_SOURCES_DIR}/fakebtgtkbackend.h
            ${STMMI_TEST_SOURCES_DIR}/fakebtgtkbackend.cc
            ${STMMI_TEST_SOURCES_DIR}/fakebtgtkdevicemanager.h
            ${STMMI_TEST_SOURCES_DIR}/fakebtgtkwindowdata.h
            ${STMMI_TEST_SOURCES_DIR}/fixtureBtDM.h
            ${STMMI_TEST_SOURCES_DIR}/fixtureGlibApp.h
            ${STMMI_TEST_SOURCES_DIR}/fixturevariantEventClasses.h
            )

    TestFiles("${STMMI_GTK_BT_TEST_SOURCES}"
              "${STMMI_SOURCES};${STMMI_GTK_BT_TEST_WITH_SOURCES}"
              "${STMMI_GTK_BT_EXTRA_LIBS}" TRUE)

    include(CTest)
endif()
