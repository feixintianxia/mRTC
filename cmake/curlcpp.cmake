include(FetchContent)
FetchContent_Declare(
        curlcpp
        GIT_REPOSITORY git@github.com:JosephP91/curlcpp.git
        GIT_TAG 3.1
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curlcpp
)

set(CURLCPP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curlcpp")
#if(NOT EXISTS "${CURLCPP_SOURCE_DIR}")
    FetchContent_Populate(
            curlcpp
            GIT_REPOSITORY git@github.com:JosephP91/curlcpp.git
            GIT_TAG 3.1
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/curlcpp
            CMAKE_ARGS
            -DBUILD_TEST:BOOL=OFF
    )
#endif()
