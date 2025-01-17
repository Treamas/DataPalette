cmake_minimum_required(VERSION 3.16)

project(DataPalette VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if DP_VENV_PATH is set in the cache
if(NOT DEFINED DP_VENV_PATH)
    message(FATAL_ERROR "DP_VENV_PATH not set. Please set it in the CMakeCache.txt or pass it as a command line argument.")
endif()

# Now you can use DP_VENV_PATH
message(STATUS "Using virtual environment path: ${DP_VENV_PATH}")

set(Python3_ROOT_DIR "${DP_VENV_PATH}")
set(Python3_EXECUTABLE "${DP_VENV_PATH}/Scripts/python.exe")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Sql)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")

set(TS_FILES DataPalette_en_GB.ts)

set(PROJECT_SOURCES
        main.cpp
        datapalettemainwindow.cpp
        datapalettemainwindow.h
        datapalettemainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DataPalette
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        cleaningwidget.h cleaningwidget.cpp cleaningwidget.ui
        homewidget.h homewidget.cpp homewidget.ui
        datawidget.h datawidget.cpp datawidget.ui
        visualizationwidget.h visualizationwidget.cpp visualizationwidget.ui
        pluginwidget.h pluginwidget.cpp pluginwidget.ui
        databasemanager.h databasemanager.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DataPalette APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(DataPalette SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DataPalette
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(DataPalette PRIVATE 
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Sql
    Python3::Python
    Python3::Module
)

target_include_directories(DataPalette PRIVATE ${Python3_INCLUDE_DIRS})
target_compile_definitions(DataPalette PRIVATE
    PYTHON_SITE_PACKAGES_PATH="${DP_VENV_PATH}/Lib/site-packages"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DataPalette)
endif()
set_target_properties(DataPalette PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DataPalette
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DataPalette)
endif()
