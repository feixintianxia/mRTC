set(WEBRTC_PREFIX_PATH "/home/li/work/webrtc/webrtc_M127/src")
set(WEBRTC_OBJ "${WEBRTC_PREFIX_PATH}/out/ubuntu-debug/obj")
set(WEBRTC_LIB "${WEBRTC_OBJ}/libwebrtc.a")
set(THIRD_PARTY_PATH "third_party")


# 如果库生成在特定目录, 添加这个目录到 PKG_CONFIG_PATH
set(OPENSSL_PKG_PATH "/home/li/work/rtc6/ffmpeg6_source/openssl-1.1.1w/build/lib/pkgconfig")
set(SDL2_PKG_PATH "/home/li/work/rtc6/ffmpeg6_source/SDL2-2.30.6/out/lib/pkgconfig")
set(CURL_PKG_PATH "/home/li/work/github/mRTCClient/mRTC/third_party/curl/out/lib/pkgconfig")
set(CURLCPP_PKG_PATH "/home/li/work/github/mRTCClient/mRTC/third_party/curlcpp/out/lib/pkgconfig")
set(JSONCPP_PKG_PATH "/home/li/work/github/mRTCClient/mRTC/third_party/jsoncpp/out/lib/pkgconfig")
set(PKG_CONFIG_PATH ${OPENSSL_PKG_PATH} ${SDL2_PKG_PATH} ${JSONCPP_PKG_PATH} ${CURL_PKG_PATH} ${CURLCPP_PKG_PATH} ${PKG_CONFIG_PATH})
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


#include_directories(${CURL_INCLUDE_DIR})


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
#include(curl)
#include(curlcpp1)
message(WARNING ${SDL2_INCLUDE_DIRS})
message(WARNING ${CURL_INCLUDE_DIRS})
message(WARNING ${CURLCPP_INCLUDE_DIRS})
message(WARNING ${SDL2_LIBRARIES})
message(WARNING ${CURL_LIBRARIES})
#include_directories(${CURL_INCLUDE_DIRS})
#include_directories(${CURLCPP_SOURCE_DIRS}/include)

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/api/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/rtc_stream/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/media/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/player/*.cpp"
)
# 创建静态库
add_library(libmRTC STATIC ${SOURCES})

target_include_directories(libmRTC PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${WEBRTC_PREFIX_PATH}
        ${WEBRTC_PREFIX_PATH}/third_party/abseil-cpp
        ${SDL2_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${CURLCPP_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
)
target_link_directories(libmRTC PUBLIC
        ${CURL_LIBRARY_DIRS}
        ${CURLCPP_LIBRARY_DIRS}
        ${JSONCPP_LIBRARY_DIRS}
)
#add_dependencies(libmRTC curlcpp)
target_link_libraries(libmRTC PUBLIC
        ${WEBRTC_LIB}
        ${GTK3_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${CURL_LIBRARIES}
        ${CURLCPP_LIBRARIES}
        ${JSONCPP_LIBRARIES}
)