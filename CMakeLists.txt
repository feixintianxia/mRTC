cmake_minimum_required(VERSION 3.28)
project(mRTC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows") # 检查是否为 Windows 系统
    add_definitions(-DWEBRTC_WIN) # 在 Windows 上定义 WEBRTC_WIN 宏
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux") # 检查是否为 Linux 系统
    add_definitions(-DWEBRTC_POSIX) # 在 Linux 上定义 WEBRTC_POSIX 宏
else ()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

set(WEBRTC_PREFIX_PATH "/home/li/work/webrtc/webrtc_M127/src")
set(WEBRTC_OBJ "${WEBRTC_PREFIX_PATH}/out/ubuntu-debug/obj")
set(WEBRTC_LIB "${WEBRTC_OBJ}/libwebrtc.a")
set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

#加载自定义模块
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#设置库文件路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#设置可执行程序路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#设置子目录
set(SUB_DIR_LIST "api" "core" "media" "player" "rtc_stream")

#安装目录
if(WIN32)
    set(INSTALL_PATH_LIB $ENV{HOME}/${PROJECT_NAME}/lib)
    set(INSTALL_PATH_INCLUDE $ENV{HOME}/${PROJECT_NAME}/include)
else()
    set(INSTALL_PATH_LIB lib)
    set(INSTALL_PATH_INCLUDE include)
endif()

foreach(SUB_DIR ${SUB_DIR_LIST})
    #遍历源文件
    aux_source_directory(src/${SUB_DIR} SRC_LIST)
    #安装头文件至系统目录
    install(DIRECTORY src/${SUB_DIR} DESTINATION ${INSTALL_PATH_INCLUDE} FILES_MATCHING PATTERN "*.h")
endforeach()


# 如果库生成在特定目录, 添加这个目录到 PKG_CONFIG_PATH
set(JSONCPP_PKG_PATH "${THIRD_PARTY_PATH}/jsoncpp/out/lib/pkgconfig")
set(OPENSSL_PKG_PATH "${THIRD_PARTY_PATH}/openssl-1.1.1w/out/lib/pkgconfig")
set(SDL2_PKG_PATH "${THIRD_PARTY_PATH}/SDL2-2.30.6/out/lib/pkgconfig")
set(CURL_PKG_PATH "${THIRD_PARTY_PATH}/curl/out/lib/pkgconfig")
set(CURLCPP_PKG_PATH "${THIRD_PARTY_PATH}/curlcpp/out/lib/pkgconfig")
set(PKG_CONFIG_PATH
        ${JSONCPP_PKG_PATH}
        ${OPENSSL_PKG_PATH}
        ${SDL2_PKG_PATH}
        ${JSONCPP_PKG_PATH}
        ${CURL_PKG_PATH}
        ${CURLCPP_PKG_PATH}
        ${PKG_CONFIG_PATH}
)
# 在 CMake 配置过程中，设置 PKG_CONFIG_PATH 环境变量
set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}")

# 查找 PkgConfig 模块
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(CURLCPP REQUIRED curlcpp)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)




set(INCLUDE_DIRS_LIST)
set(LIBRARIES_LIST)

#查找jsoncpp是否安装
if(JSONCPP_FOUND)
    message(STATUS "找到jsoncpp库:\"${JSONCPP_INCLUDE_DIRS}\"")
    list(APPEND  INCLUDE_DIRS_LIST ${JSONCPP_INCLUDE_DIRS})
    list(APPEND  LIBRARIES_LIST ${JSONCPP_LIBRARIES})
else()
    message(FATAL_ERROR "找不到jsoncpp库:\"")
endif()

#查找sdl2是否安装
if(SDL2_FOUND)
    message(STATUS "找到sdl2库:\"${SDL2_INCLUDE_DIRS}\"")
    list(APPEND  INCLUDE_DIRS_LIST ${SDL2_INCLUDE_DIRS})
    list(APPEND  LIBRARIES_LIST ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "找不到sdl2库:\"")
endif()

#查找openssl是否安装
if(OPENSSL_FOUND)
    message(STATUS "找到openssl库:\"${OPENSSL_INCLUDE_DIRS}\"")
    list(APPEND  INCLUDE_DIRS_LIST ${OPENSSL_INCLUDE_DIRS})
    list(APPEND  LIBRARIES_LIST ${OPENSSL_LIBRARIES})
else()
    message(FATAL_ERROR "找不到openssl库:\"")
endif()

#查找curl是否安装
if(CURL_FOUND)
    message(STATUS "找到curl库:\"${CURL_INCLUDE_DIRS}\"")
    list(APPEND  INCLUDE_DIRS_LIST ${CURL_INCLUDE_DIRS})
    list(APPEND  LIBRARIES_LIST ${CURL_LIBRARIES})
else()
    message(FATAL_ERROR "找不到curl库:\"")
endif()

#查找curlcpp是否安装
if(CURLCPP_FOUND)
    message(STATUS "找到curlcpp库:\"${CURLCPP_INCLUDE_DIRS}\"")
    list(APPEND  INCLUDE_DIRS_LIST ${CURLCPP_INCLUDE_DIRS})
    list(APPEND  LIBRARIES_LIST ${CURLCPP_LIBRARIES})
else()
    message(FATAL_ERROR "找不到curlcpp库:\"")
endif()

#添加webrtc
list(APPEND  INCLUDE_DIRS_LIST ${WEBRTC_PREFIX_PATH} ${WEBRTC_PREFIX_PATH}/third_party/abseil-cpp)
list(APPEND  LIBRARIES_LIST ${WEBRTC_LIB} ${GTK3_LIBRARIES})


message(STATUS "将依赖头文件:${INCLUDE_DIRS_LIST}")
message(STATUS "将链接依赖库:${LIBRARIES_LIST}")

if(NOT WIN32)
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wno-predefined-identifier-outside-function)
endif()

#编译静态库
add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${INCLUDE_DIRS_LIST})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
#install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${INSTALL_PATH_LIB})


set(BUILD_EXAMPELES ON CACHE BOOL "enable build examples")
#示例程序
if(BUILD_EXAMPELES)
    add_subdirectory(examples/QtClient)
endif()

