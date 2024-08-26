include(FetchContent)
#FetchContent_Declare(
#        curl
#        GIT_REPOSITORY git@github.com:curl/curl.git
#        GIT_TAG curl-8_9_1
#        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curl
#)

set(CURL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curl")
#if(NOT EXISTS "${CURL_SOURCE_DIR}")
    #FetchContent_MakeAvailable(curl)
    FetchContent_Populate(
            curl
            GIT_REPOSITORY git@github.com:curl/curl.git
            GIT_TAG curl-8_9_1
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curl
            CMAKE_ARGS
            -DBUILD_LIBCURL_DOCS:BOOL=OFF
            -DBUILD_EXAMPLES:BOOL=OFF
            -DHAVE_MANUAL_TOOLS:BOOL=OFF
    )
#endif()