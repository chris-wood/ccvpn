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

# Store process PIDs to shut them down later...
PIDS=()

func start_consumer() {
    INDEX=$1
    PREFIX=${COMMON_PREFIX}/${INDEX}

    echo Starting consumer at ${PREFIX}
    
    ${CLIENT_BINARY} -l ${PREFIX} -c ${NUMBER_PACKETS} -s ${RESPONSE_SIZE} -f ${PACKET_RATE} &
    PID=$!
    PIDS+=(${PID})
}

for i in `seq 1 ${NUMBER_CLIENTS}`;
do
    start_consumer ${i}
done   

echo "Press any key to kill the servers..."
read killswitch

for i in `seq 1 ${NUMBER_CLIENTS}`;
do
    kill -INT ${PIDS[$i]}
done

