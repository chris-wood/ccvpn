#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv

SERVERS=1

### N x N ###
for i in `seq 0 9`;
do
    CLIENTS=$((10 + 10 * i))

    sh experiments_clean.sh
    sleep 2s
    sh setup_network.sh $SERVERS sk &
    sleep 10s

    sh run_clients.sh $CLIENTS $SERVERS $PACKET_SIZE sk
    sleep 2s
done

### N x N ###
for i in `seq 0 9`;
do
    CLIENTS=$((10 + 10 * i))

    sh experiments_clean.sh
    sleep 2s
    sh setup_network.sh $CLIENTS pk &
    sleep 10s

    sh run_clients.sh $CLIENTS $SERVERS $PACKET_SIZE pk
    sleep 2s
done

cp *sk*.csv ../experiments/symm_key
rm *sk*.png
rm *sk*.csv

cp *pk*.csv ../experiments/public_key
rm *pk*.png
rm *pk*.csv

