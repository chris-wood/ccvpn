#!/usr/bin/bash

SERVER_BINARY=$1
COMMON_PREFIX=$2
NUMBER_PRODUCERS=$3

# Producer default port
export METIS_PORT=9695

# Store process PIDs to shut them down later...
PIDS=()

func start_producer() {
    INDEX=$1
    PREFIX=${COMMON_PREFIX}/${INDEX}

    echo Starting producer at ${PREFIX}
    
    ${SERVER_BINARY} -l ${PREFIX} &
    PID=$!
    PIDS+=(${PID})
}

for i in `seq 1 ${NUMBER_PRODUCERS}`;
do
    start_producer ${i}
done   

echo "Press any key to kill the servers..."
read killswitch

for i in `seq 1 ${NUMBER_PRODUCERS}`;
do
    kill -INT ${PIDS[$i]}
done

