cmake_minimum_required(VERSION 3.16)
project(AudioBridge VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include CMake modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Find required packages
# First try Qt5, which is more commonly available
find_package(Qt5 COMPONENTS Core Widgets Network Multimedia QUIET)
if (Qt5_FOUND)
    message(STATUS "Found Qt5")
    set(QT_VERSION_MAJOR 5)
else()
    # Try Qt6 if Qt5 is not found
    find_package(Qt6 COMPONENTS Core Widgets Network Multimedia QUIET)
    if (Qt6_FOUND)
        message(STATUS "Found Qt6")
        set(QT_VERSION_MAJOR 6)
    else()
        message(FATAL_ERROR "Neither Qt5 nor Qt6 was found. Please install Qt development libraries.")
    endif()
endif()

# Find PortAudio package
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

# Set automoc, autorcc, autouic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PORTAUDIO_INCLUDE_DIRS}
)

# Add source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/audiomanager.cpp
    src/networkmanager.cpp
)

# Add header files
set(HEADERS
    include/mainwindow.h
    include/audiomanager.h
    include/networkmanager.h
)

# Add UI files
set(UI_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mainwindow.ui
)

# Set UI include directory for generated ui_*.h files
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# Make sure the build system can find the UI files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Network
    Qt::Multimedia
    ${PORTAUDIO_LIBRARIES}
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
