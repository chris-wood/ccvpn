#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv
sh experiments_clean.sh
sleep 2s

##SETUP A NETOWORK WITH 4 producers in symm key  mode
sh setup_network.sh 4 sk &
sleep 2s

################################ N x 1 client servers experiments for symm key setup
sh run_clients_n_to_1.sh 1 1 $PACKET_SIZE sk
sleep 2s

sh run_clients_n_to_1.sh 2 1 $PACKET_SIZE sk
sleep 2s

sh run_clients_n_to_1.sh 3 1 $PACKET_SIZE sk
sleep 2s

sh run_clients_n_to_1.sh 4 1 $PACKET_SIZE sk
sleep 2s
################################ N X N client servers experiments for symm key setup
sh run_clients.sh 2 2 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 3 3 $PACKET_SIZE sk
sleep 2s

sh run_clients.sh 4 4 $PACKET_SIZE sk
sleep 2s

cp *sk*.png ../experiments/symm_key
cp *sk*.csv ../experiments/symm_key
sleep 10

##SETUP NETWORK W/ 4 procuccers in public key mode
sh experiments_clean.sh
sleep 2s
sh setup_network.sh 4 pk &
sleep 2s

################################ N x 1 client servers experiments for public key setup
sh run_clients_n_to_1.sh 1 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 2 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 3 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 4 1 $PACKET_SIZE pk
sleep 2s
################################ N X N client servers experiments for public key setup
sh run_clients.sh 1 1 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 2 2 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 3 3 $PACKET_SIZE pk
sleep 2s

sh run_clients.sh 4 4 $PACKET_SIZE pk
sleep 2s

sh experiments_clean.sh

cp *pk*.png ../experiments/public_key
cp *pk*.csv ../experiments/public_key

