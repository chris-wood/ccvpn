#!/usr/bin/bash

SERVER_BINARY=$1
COMMON_PREFIX=$2
NUMBER_PRODUCERS=$3

# Producer default port
export METIS_PORT=9695

# Store process PIDs to shut them down later...3
PIDS=

StartProducer() {
    INDEX=$1
    PREFIX=${COMMON_PREFIX}/${INDEX}

    echo Starting producer at ${PREFIX}
    
    ${SERVER_BINARY} -l ${PREFIX} &
    PID=$!
    echo $PID
    PIDS+=$($PID)
    echo $PIDS
}

for i in `seq 1 ${NUMBER_PRODUCERS}`;
do
    StartProducer ${i}
done   

echo "Press any key to kill the servers..."
read killswitch

killall "ccnxVPN_Server"

#for i in `seq 1 ${NUMBER_PRODUCERS}`;
#do
#    kill -INT ${PIDS[$i]}
#done

