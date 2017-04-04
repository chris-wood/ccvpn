#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv

SERVERS=1

### N x N ###
for i in `seq 0 5`;
do
    CLIENTS=$((20 + 20 * i))
    SERVERS=$((20 + 20 * i))
    sh experiments_clean.sh
    sleep 2s
    sh setup_network.sh $SERVERS sk &
    sleep 10s

    sh run_clients_many_servers.sh $CLIENTS $SERVERS $PACKET_SIZE sk
    sleep 2s
done

### N x N ###
for i in `seq 0 5`;
do
    CLIENTS=$((20 + 20 * i))
    SERVERS=$((20 + 20 * i))

    sh experiments_clean.sh
    sleep 2s
    sh setup_network.sh $CLIENTS pk &
    sleep 10s

    sh run_clients_many_servers.sh $CLIENTS $SERVERS $PACKET_SIZE pk
    sleep 2s
done

cp *sk*.csv ../experiments/symm_key
rm *sk*.png
rm *sk*.csv

cp *pk*.csv ../experiments/public_key
rm *pk*.png
rm *pk*.csv

