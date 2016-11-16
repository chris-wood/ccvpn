cmake_minimum_required(VERSION 3.2)
project(ccnxVPN)
include( CTest )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CCNX_HOME $ENV{CCNX_HOME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(CCNX_LIBRARIES longbow longbow-ansiterm parc ccnx_common ccnx_api_portal ccnx_transport_rta ccnx_api_control ccnx_api_notify sodium)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

set(CONSUMER_SOURCES
        consumer/ccnxVPN_Consumer.c
        ccnxVPN_Common.c
        ccnxVPN_Stats.c)

set(PRODUCER_SOURCES
        producer/ccnxVPN_Producer.c
        ccnxVPN_Common.c)

set(KEYGEN_SOURCES
    athena_keygen.c)

include_directories(${CCNX_HOME}/include)

link_directories(${CCNX_HOME}/lib)

add_executable(ccnxVPN_Client ${CONSUMER_SOURCES})
target_link_libraries(ccnxVPN_Client ${CCNX_LIBRARIES})
install(TARGETS ccnxVPN_Client RUNTIME DESTINATION bin)

add_executable(ccnxVPN_Server ${PRODUCER_SOURCES})
target_link_libraries(ccnxVPN_Server ${CCNX_LIBRARIES})
install(TARGETS ccnxVPN_Server RUNTIME DESTINATION bin)

add_executable(keygen ${KEYGEN_SOURCES})
target_link_libraries(keygen ${CCNX_LIBRARIES} sodium)
install(TARGETS keygen RUNTIME DESTINATION bin)

add_subdirectory(gateway)

add_test(EmptyTest, echo "OK")
