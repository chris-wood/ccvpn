#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv

##SETUP A NETOWORK WITH 1 producers in symm key  mode
sh experiments_clean.sh
sleep 2s
sh setup_network.sh 1 sk &
sleep 10s
################################ N x 1 client servers experiments for symm key setup

sh run_rates.sh 1 1 $PACKET_SIZE sk
sleep 2s

cp *sk*.csv ../experiments/symm_key
rm *sk*.png
rm *sk*.csv

##SETUP NETWORK W/ 1 procuccers in public key mode
sh experiments_clean.sh
sleep 2s
sh setup_network.sh 1 pk &
sleep 10s

################################ N x 1 client servers experiments for public key setup

sh run_rates.sh 1 1 $PACKET_SIZE pk
sleep 2s

sh experiments_clean.sh

cp *pk*.csv ../experiments/public_key
rm *pk*.png
rm *pk*.csv

