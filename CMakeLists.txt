cmake_minimum_required(VERSION 3.14)
project(IoT2Phone)
set(CMAKE_CXX_STANDARD 17)

option(DTLS "Enable DTLS" OFF)

include(${CMAKE_CURRENT_LIST_DIR}/include/wakaama/core/wakaama.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/include/wakaama/examples/shared/shared.cmake)

add_definitions(-DLWM2M_CLIENT_MODE)
if(LWM2M_VERSION VERSION_GREATER "1.0")
    add_definitions(-DLWM2M_SUPPORT_SENML_JSON)
else()
    add_definitions(-DLWM2M_SUPPORT_JSON)
endif()

add_definitions(${SHARED_DEFINITIONS} ${WAKAAMA_DEFINITIONS})

include_directories(${WAKAAMA_SOURCES_DIR} ${SHARED_INCLUDE_DIRS})

set(SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/connection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/connection.h
        object.cpp object.h objectsecurity.cpp objectsecurity.h objectserver.cpp objectserver.h objectdevice.cpp objectdevice.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${WAKAAMA_SOURCES} ${SHARED_SOURCES})

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:WITH_LOGS>)

#//////////////////////////////////////Include Boost for boost.asio//////////////////////////////
find_package(Boost 1.67.0 COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if(Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_system -fopenmp")
endif()
#//////////////////////////////////////End of boost definations//////////////////////////////