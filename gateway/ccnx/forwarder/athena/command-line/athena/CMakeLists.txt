set(ATHENA_SOURCE_FILES
    athena_main.c
    #../../athena_TransportLinkModuleTCP.c # uncomment this line to statically include the TCP link module
    #../../athena_TransportLinkModuleUDP.c # uncomment this line to statically include the UDP link module
    #../../athena_TransportLinkModuleETH.c # uncomment this line to statically include the ETH link module
    )

add_executable(athenabin ${ATHENA_SOURCE_FILES})
target_link_libraries(athenabin ${ATHENA_LINK_LIBRARIES} sodium)
set_target_properties(athenabin PROPERTIES OUTPUT_NAME athena)

install(TARGETS athenabin RUNTIME DESTINATION bin)

## We might want to consider trying out each of the different links to measure their throughput

set(ATHENA_PRIVATE_SOURCE_FILES
        athena_private.c
        #../../athena_TransportLinkModuleTCP.c # uncomment this line to statically include the TCP link module
        #../../athena_TransportLinkModuleUDP.c # uncomment this line to statically include the UDP link module
        #../../athena_TransportLinkModuleETH.c # uncomment this line to statically include the ETH link module
        )

add_executable(athena_private_bin ${ATHENA_PRIVATE_SOURCE_FILES})
target_link_libraries(athena_private_bin ${ATHENA_LINK_LIBRARIES} sodium)
set_target_properties(athena_private_bin PROPERTIES OUTPUT_NAME athena_private)

install(TARGETS athena_private_bin RUNTIME DESTINATION bin)

set(ATHENA_GATEWAY_SOURCE_FILES
        athena_gateway.c
        #../../athena_TransportLinkModuleTCP.c # uncomment this line to statically include the TCP link module
        #../../athena_TransportLinkModuleUDP.c # uncomment this line to statically include the UDP link module
        #../../athena_TransportLinkModuleETH.c # uncomment this line to statically include the ETH link module
        )

add_executable(athena_gateway_bin ${ATHENA_GATEWAY_SOURCE_FILES})
target_link_libraries(athena_gateway_bin ${ATHENA_LINK_LIBRARIES} sodium)
set_target_properties(athena_gateway_bin PROPERTIES OUTPUT_NAME athena_gateway)

install(TARGETS athena_gateway_bin RUNTIME DESTINATION bin)
