cmake_minimum_required(VERSION 3.21.1)

set(BUILD_QDS_COMPONENTS ON CACHE BOOL "Build design studio components")
option(LINK_INSIGHT "Link Qt Insight Tracker library" ON)

project(Raise73App LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 COMPONENTS Gui Qml Quick)
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)
find_package(Qt6 REQUIRED COMPONENTS  Sql LinguistTools)

# To build this application you need Qt 6.2.0 or higher

if (Qt6_VERSION VERSION_LESS 6.2.0)
message(FATAL_ERROR "You need Qt 6.2.0 or newer to build the application.")
endif()

if (ANDROID)
    message(STATUS "Configuring for Android")
    # Add linker flag to ignore duplicate symbols
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -z muldefs")
endif()

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES
    "src/*.c"
    "src/*.cpp"
    "src/*.h"
    "src/*.hpp"
)

qt_add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
    src
    src/engine
    src/third_party
    src/third_party/poker-eval/include
)

# qt_standard_project_setup() requires Qt 6.3 or higher. See https://doc.qt.io/qt-6/qt-standard-project-setup.html for details.
if (${QT_VERSION_MINOR} GREATER_EQUAL 3)
    qt6_standard_project_setup()
endif()

qt_add_resources(${CMAKE_PROJECT_NAME} "configuration"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Sql
)

set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/imports ${CMAKE_BINARY_DIR}/imports CACHE STRING "" FORCE)

# Add the QML_IMPORT_PATH to the QML engine
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    QML_IMPORT_PATH "${QML_IMPORT_PATH}"
)

if (${BUILD_QDS_COMPONENTS})
    include(${CMAKE_CURRENT_SOURCE_DIR}/qmlcomponents)
endif ()

include(${CMAKE_CURRENT_SOURCE_DIR}/qmlmodules)

if (LINK_INSIGHT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/insight)
endif ()

install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

