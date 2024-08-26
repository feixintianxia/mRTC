include(ExternalProject)
set(CURL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curl")
set(CURLCPP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curlcpp")
ExternalProject_Add(curl
        GIT_REPOSITORY      git@github.com:curl/curl.git
        GIT_TAG             curl-8_9_1
        SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curl
        GIT_PROGRESS        true
        CMAKE_ARGS          "-DBUILD_EXAMPLES:BOOL=OFF"
        #STEP_TARGETS   build
        INSTALL_COMMAND ""
)

add_library(curl::curl INTERFACE IMPORTED GLOBAL)
add_dependencies(curl::curl curl)  # to avoid include directory race condition
target_link_libraries(curl::curl INTERFACE ${CURL_LIBRARIES})
target_include_directories(curl::curl INTERFACE ${CURL_INCLUDE_DIR})

message(WARNING ${curl_INCLUDE_DIRS} "++++++++++10")
find_package(curl)
message(WARNING ${curl_INCLUDE_DIRS} "++++++++++11")
ExternalProject_Add(curlcpp
        GIT_REPOSITORY      git@github.com:JosephP91/curlcpp.git
        GIT_TAG             3.1
        SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curlcpp
        GIT_PROGRESS        true
        CMAKE_ARGS          "-DBUILD_TEST:BOOL=OFF"
        DEPENDS             curl
        #STEP_TARGETS        build
        INSTALL_COMMAND ""

)
