#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv
sh experiments_clean.sh
sleep 2s

##SETUP A NETOWORK WITH 1 producers in symm key  mode
sh setup_network.sh 140 sk &
sleep 5s
################################ N x 1 client servers experiments for symm key setup

sh run_clients.sh 20 20 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 40 40 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 60 60 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 80 80 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 100 100 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 120 120 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 140 140 $PACKET_SIZE sk
sleep 2s

cp *sk*.csv ../experiments/symm_key
rm *sk*.png
rm *sk*.csv

##SETUP NETWORK W/ 1 procuccers in public key mode
sh experiments_clean.sh
sleep 2s
sh setup_network.sh 140 pk &
sleep 5s

################################ N x 1 client servers experiments for public key setup

sh run_clients.sh 20 20 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 40 40 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 60 60 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 80 80 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 100 100 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 120 120 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 140 140 $PACKET_SIZE pk
sleep 2s

sh experiments_clean.sh

cp *pk*.csv ../experiments/public_key
rm *pk*.png
rm *pk*.csv

