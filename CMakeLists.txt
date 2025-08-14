cmake_minimum_required(VERSION 3.22)
project(shun4mishu VERSION 1.0 DESCRIPTION "A Discord bot with Chinese")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/env_parser/env_parser.cpp
    src/converter/converter.cpp
)

find_package(PkgConfig REQUIRED)
# pkg_check_modules(LIBPINYIN REQUIRED libpinyin)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(DPP REQUIRED)
target_link_libraries(shun4mishu PRIVATE ${DPP_LIBRARIES})
# target_include_directories(shun4mishu PRIVATE ${LIBPINYIN_INCLUDE_DIRS})
target_include_directories(shun4mishu PRIVATE ${DPP_INCLUDE_DIR})
# target_link_libraries(test_pinyin PRIVATE ${LIBPINYIN_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
