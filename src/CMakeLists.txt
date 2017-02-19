# QTDIR must point to c:/Qt/5.7/mingw53_32/ or similar
message("qtdir is set to '$ENV{QTDIR}' (must be 'c:/Qt/5.7/mingw53_32/' or similar)")
set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(THOR_NAME "thor")

include_directories(.)

set(THOR_SOURCES
        main.cpp
        thor.ui
        MainWindow.cpp
        MainWindow.h
        GLWidget.cpp
        GLWidget.h mesh.cpp mesh.h)

add_executable(${THOR_NAME} ${THOR_SOURCES})
target_link_libraries(${THOR_NAME} Qt5::Widgets Qt5::Core)
