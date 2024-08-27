cmake_minimum_required(VERSION 3.28)
project(QtClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_PATH "/home/li/Qt/6.7.2/gcc_64/lib/cmake" CACHE PATH "qt6 cmake dir")
set(CMAKE_CXX_FLAGS "-g -Wall  -Wl,--copy-dt-needed-entries ${ERIZO_CMAKE_CXX_FLAGS}")
list(APPEND CMAKE_PREFIX_PATH ${QT_PATH})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows") # 检查是否为 Windows 系统
    add_definitions(-DWEBRTC_WIN) # 在 Windows 上定义 WEBRTC_WIN 宏
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux") # 检查是否为 Linux 系统
    add_definitions(-DWEBRTC_POSIX) # 在 Linux 上定义 WEBRTC_POSIX 宏
else ()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

#add_definitions(-DWdeprecated-declarations) # 在 Linux 上定义 WEBRTC_POSIX 宏
#Qt相关
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



set(SOURCES
        main.cpp
        QtClient.cpp
)

set(HEADERS
        QtClient.h
)

set(UIS
        QtWidget.ui
)


qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${SOURCES} ${HEADERS} ${UIS}
)

target_link_libraries(${PROJECT_NAME}  PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        libmRTC
)


