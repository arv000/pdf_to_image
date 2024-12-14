cmake_minimum_required(VERSION 3.16)

project(PDFToImg VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置静态运行时库
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

find_package(PkgConfig)
pkg_check_modules(POPPLER_CPP REQUIRED IMPORTED_TARGET poppler-cpp)

#add_compile_definitions(QT_STATIC)
#set(TS_FILES PDFToImg_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        workthread.h
        workthread.cpp
)
set(QRC_SOURCE_FILES resources.qrc)
add_executable(PDFToImg WIN32 ${PROJECT_SOURCES} ${QRC_SOURCE_FILES})

target_link_libraries(PDFToImg PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(PDFToImg PRIVATE PkgConfig::POPPLER_CPP)
