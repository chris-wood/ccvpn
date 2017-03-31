#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv
sh experiments_clean.sh
sleep 2s

##SETUP A NETOWORK WITH 1 producers in symm key  mode
sh setup_network.sh 300 sk &
sleep 10s
################################ N x 1 client servers experiments for symm key setup

sh run_clients.sh 10 10 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 50 50 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 100 100 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 150 150 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 200 200 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 250 250 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 300 300 $PACKET_SIZE sk
sleep 2s

cp *sk*.csv ../experiments/symm_key
rm *sk*.png
rm *sk*.csv

##SETUP NETWORK W/ 1 procuccers in public key mode
sh experiments_clean.sh
sleep 2s
sh setup_network.sh 300 pk &
sleep 10s

################################ N x 1 client servers experiments for public key setup

sh run_clients.sh 10 10 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 50 50 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 100 100 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 150 150 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 200 200 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 250 250 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 300 300 $PACKET_SIZE pk
sleep 2s

sh experiments_clean.sh

cp *pk*.csv ../experiments/public_key
rm *pk*.png
rm *pk*.csv

