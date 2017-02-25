#!/usr/bin/bash

SERVER_BINARY=$1
COMMON_PREFIX=$2
NUMBER_PRODUCERS=$3

# Identity file name and password prefixes
IDENTITY_PREFIX="producer_identity"

# Producer default port
export METIS_PORT=9695

StartProducer() {
    INDEX=$1
    PREFIX=${COMMON_PREFIX}/${INDEX}

    IDENTITY_FILE=${IDENTITY_PREFIX}_${INDEX}
    IDENTITY_PASS=${IDENTITY_FILE}

    echo Starting producer at ${PREFIX}

    nice -n -15 ${SERVER_BINARY} -l ${PREFIX} -i ${IDENTITY_FILE} -p ${IDENTITY_PASS} &
    PID=$!
    #echo $PID
}

for i in `seq 1 ${NUMBER_PRODUCERS}`;
do
    StartProducer ${i}
done

#echo "Press any key to kill the servers..."
#read killswitch

#killall "ccnxVPN_Server"

#for i in `seq 1 ${NUMBER_PRODUCERS}`;
#do
#    kill -INT ${PIDS[$i]}
#done
