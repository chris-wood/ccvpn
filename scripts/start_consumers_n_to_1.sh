#!/usr/bin/bash

CLIENT_BINARY=$1
COMMON_PREFIX=$2

NUMBER_CLIENTS=$3

# Packet rate == rate at which packets will be sent
PACKET_RATE=$4

# Response size == size of the content object responses from the producer
RESPONSE_SIZE=$5

# Number of packets == total number of packets to send by the client
NUMBER_PACKETS=$6

# Consumer default port
export METIS_PORT=9696

StartConsumer() {
    INDEX=$1
    PREFIX=${COMMON_PREFIX}/${INDEX}
    echo Starting consumer at ${PREFIX}
    ${CLIENT_BINARY} -l ${PREFIX} -c ${NUMBER_PACKETS} -s ${RESPONSE_SIZE} -f ${PACKET_RATE} &
}

for i in `seq 1 ${NUMBER_CLIENTS}`;
do
    StartConsumer 1
#${i}
done
wait;

#echo "Press any key to quit..."
#read killswitch

#${CLIENT_BINARY} -l ccnx:/producer/kill -c ${NUMBER_PACKETS} -s ${RESPONSE_SIZE} -f ${PACKET_RATE} &

#sleep 2s
 
killall "ccnxVPN_Client"

