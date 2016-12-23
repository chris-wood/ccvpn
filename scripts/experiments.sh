#!/usr/bin/bash
#content packet size
PACKET_SIZE=$1

rm *.png
rm *.csv
sh experiments_clean.sh
sleep 1s

################################ N x 1 client servers experiments for symm key setup
sh setup_network.sh 1 sk &
sleep 5s
sh run_clients_n_to_1.sh 1 1 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 1 sk &
sleep 5s
sh run_clients_n_to_1.sh 2 1 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 1 sk &
sleep 5s
sh run_clients_n_to_1.sh 3 1 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 1 sk &
sleep 5s
sh run_clients_n_to_1.sh 4 1 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s
################################ N x 1 client servers experiments for public key setup
sh setup_network.sh 1 pk &
sleep 5s
sh run_clients_n_to_1.sh 1 1 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 1 pk &
sleep 5s
sh run_clients_n_to_1.sh 2 1 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 1 pk &
sleep 5s
sh run_clients_n_to_1.sh 3 1 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 1 pk &
sleep 5s
sh run_clients_n_to_1.sh 4 1 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s
################################ N X N client servers experiments for symm key setup
sh setup_network.sh 1 sk &
sleep 5s
sh run_clients.sh 1 1 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 2 sk &
sleep 5s
sh run_clients.sh 2 2 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 3 sk &
sleep 5s
sh run_clients.sh 3 3 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 4 sk &
sleep 5s
sh run_clients.sh 4 4 $PACKET_SIZE sk
sh experiments_clean.sh
sleep 1s
################################ N X N client servers experiments for public key setup
sh setup_network.sh 1 pk &
sleep 5s
sh run_clients.sh 1 1 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 2 pk &
sleep 5s
sh run_clients.sh 2 2 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 3 pk &
sleep 5s
sh run_clients.sh 3 3 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s

sh setup_network.sh 4 pk &
sleep 5s
sh run_clients.sh 4 4 $PACKET_SIZE pk
sh experiments_clean.sh
sleep 1s
