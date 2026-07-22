cmake_minimum_required(VERSION 3.16)

project(Crocodile VERSION 0.1 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(QT_VERSION_MAJOR 6)

enable_testing()
add_subdirectory(logic)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_SOURCES
        main.cpp
        forms/src/mainwindow.cpp
        forms/src/joindialog.cpp
        forms/src/gamewindow.cpp
        forms/src/createdialog.cpp
        forms/src/paintwidget.cpp
        resources.qrc
)

set(PROJECT_HEADERS
        forms/include/mainwindow.h
        forms/include/joindialog.h
        forms/include/gamewindow.h
        forms/include/createdialog.h
        forms/include/paintwidget.h
)

set(PROJECT_FORMS
        forms/ui/mainwindow.ui
        forms/ui/joindialog.ui
        forms/ui/gamewindow.ui
        forms/ui/createdialog.ui
        forms/ui/paintwidget.ui
)


if(WIN32)
    set(app_icon_resource "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
endif()
        
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Crocodile
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_FORMS}
        ${app_icon_resource}
    )
else()
    if(ANDROID)
        add_library(Crocodile SHARED 
        ${PROJECT_SOURCES} 
        ${PROJECT_FORMS}
        ${PROJECT_HEADERS})
    else()
        add_executable(Crocodile ${PROJECT_SOURCES})
    endif()
endif()

set_target_properties(Crocodile PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/forms/ui"
)
target_include_directories(Crocodile PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/forms/include
)

target_link_libraries(Crocodile PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    crocodile_back
)

if(MSVC)
    target_compile_options(Crocodile PRIVATE /Zc:__cplusplus)
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(Crocodile PROPERTIES
        BUILD_RPATH "$ORIGIN:$ORIGIN/lib"
        INSTALL_RPATH "$ORIGIN:$ORIGIN/lib"
    )
endif()

if("${Qt6_VERSION}" VERSION_LESS "6.1.0")
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Crocodile)
endif()

set_target_properties(Crocodile PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Crocodile
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Crocodile)
endif()