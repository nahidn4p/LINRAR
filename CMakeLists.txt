cmake_minimum_required(VERSION 3.16)
project(LINRAR VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 (preferred) or Qt5 (fallback)
set(QT_MIN_VERSION "6.0.0")
find_package(Qt6 ${QT_MIN_VERSION} QUIET COMPONENTS Core Widgets Gui)

if(NOT Qt6_FOUND)
    message(STATUS "Qt6 not found, trying Qt5...")
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets Gui)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
    message(STATUS "Found Qt6")
endif()

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ArchiveView.cpp
    src/FileBrowser.cpp
    src/ArchiveHandler.cpp
    src/handlers/RarHandler.cpp
    src/handlers/ZipHandler.cpp
    src/handlers/SevenZipHandler.cpp
    src/handlers/TarHandler.cpp
    src/ArchiveModel.cpp
    src/ProcessManager.cpp
    src/SettingsManager.cpp
    src/ProgressDialog.cpp
    src/utils/ArchiveUtils.cpp
    src/utils/FormatDetector.cpp
)

set(HEADERS
    src/MainWindow.h
    src/ArchiveView.h
    src/FileBrowser.h
    src/ArchiveHandler.h
    src/handlers/RarHandler.h
    src/handlers/ZipHandler.h
    src/handlers/SevenZipHandler.h
    src/handlers/TarHandler.h
    src/ArchiveModel.h
    src/ProcessManager.h
    src/SettingsManager.h
    src/ProgressDialog.h
    src/utils/ArchiveUtils.h
    src/utils/FormatDetector.h
)

# UI files
set(UI_FILES
    ui/MainWindow.ui
)

# Resource files
set(RESOURCES
    resources/linrar.qrc
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# Link Qt libraries
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} 
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
    )
else()
    target_link_libraries(${PROJECT_NAME} 
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Install desktop file
install(FILES packaging/appimage/linrar.desktop
        DESTINATION share/applications)

# Install icons (if they exist)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/linrar.png")
    install(FILES assets/icons/linrar.png
            DESTINATION share/pixmaps
            RENAME linrar.png)
endif()

# CPack configuration - MUST be set BEFORE include(CPack)
set(CPACK_PACKAGE_NAME "linrar")
set(CPACK_PACKAGE_VENDOR "LINRAR")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Linux Archive Manager - WinRAR alternative for Linux")
set(CPACK_PACKAGE_DESCRIPTION "LINRAR is a modern Qt-based archive manager for Linux that provides WinRAR-like functionality by interfacing with native command-line tools (rar, 7zip, tar, etc.).")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "LINRAR Team <linrar@example.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_NAME "linrar")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "LINRAR Team <linrar@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
if(QT_VERSION_MAJOR EQUAL 6)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6 (>= 6.0.0), libqt6widgets6 (>= 6.0.0), libqt6gui6 (>= 6.0.0), rar | unrar, p7zip-full | 7z, zip, unzip, tar")
else()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a (>= 5.15.0), libqt5widgets5 (>= 5.15.0), libqt5gui5 (>= 5.15.0), rar | unrar, p7zip-full | 7z, zip, unzip, tar")
endif()
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

# RPM package configuration
set(CPACK_RPM_PACKAGE_NAME "linrar")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Archiving")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_VENDOR "LINRAR")
if(QT_VERSION_MAJOR EQUAL 6)
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.0.0, rar, p7zip, zip, unzip, tar")
else()
    set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase >= 5.15.0, rar, p7zip, zip, unzip, tar")
endif()
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

# Source package
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/build/;/\.git/;/\.gitignore;/\.vscode/;/\.idea/;.*\.swp;.*\.swo;~$"
)

# Include CPack AFTER all configuration is set
include(CPack)
