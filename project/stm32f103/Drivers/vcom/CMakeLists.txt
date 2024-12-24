if ("${DRIVER_NAME}" STREQUAL "")
    get_filename_component(DRIVER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(APPEND DRIVER_NAME "_driver")
endif()

set(DRIVER_PROJECT_NAME ${DRIVER_NAME})

project(${DRIVER_PROJECT_NAME})

add_library(${DRIVER_PROJECT_NAME} INTERFACE)

file(GLOB DRIVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

set(DRIVER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_sources(${DRIVER_PROJECT_NAME} INTERFACE ${DRIVER_SOURCES})

target_include_directories(${DRIVER_PROJECT_NAME} INTERFACE ${DRIVER_INCLUDE_DIRS})

target_link_libraries(${DRIVER_PROJECT_NAME} INTERFACE stm32cubemx)

