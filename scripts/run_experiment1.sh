#!/usr/bin/bash

sh setup_network.sh 1 &
sleep 3s
sh run_clients.sh 1 1 4096 1 &

echo "Press any key to quit..."
read killswitch

../b/ccnxVPN_Client -l ccnx:/producer/kill -c 1 -s 4096 -f 1

sleep 1s
 
killall "ccnxVPN_Client"

