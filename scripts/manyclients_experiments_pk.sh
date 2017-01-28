PACKET_SIZE=$1

rm *pk*.png
rm *pk*.csv
sh experiments_clean.sh
sleep 2s

##SETUP A NETOWORK WITH 1 producers in symm key  mode
sh setup_network.sh 1 sk &
sleep 2s


################################ N x 1 client servers experiments for public key setup

sh run_clients_n_to_1.sh 200 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 400 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 600 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 800 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 1000 1 $PACKET_SIZE pk
sleep 2s

sh run_clients_n_to_1.sh 1200 1 $PACKET_SIZE pk
sleep 2s

sh experiments_clean.sh

cp *pk*.png ../experiments/public_key
cp *pk*.csv ../experiments/public_key
