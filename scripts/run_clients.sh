#!/usr/bin/bash

echo "\n\nUsage: sh run_clients.sh <NUMBER_CLIENTS> <NUMBER_SERVERS> <RESPONSE_SIZE> <MODE>\n"
echo "setup_network.sh must be running with the same <NUMBER_PRODUCERS> parameter\n\n"

echo "intReg,intEncap,intDecap,contReg,contEnc,contDec" > times.csv

chmod +w times.csv

NUMBER_CLIENTS=$1
NUMBER_SERVERS=$2
RESPONSE_SIZE=$3
MODE=$4

echo "n_pkt,thput" > throughput.csv
for i in `seq 1 10`;
do
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 5 $RESPONSE_SIZE 5
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 25 $RESPONSE_SIZE 25
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 50 $RESPONSE_SIZE 50
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 100 $RESPONSE_SIZE 100
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 200 $RESPONSE_SIZE 200
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 400 $RESPONSE_SIZE 400
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 800 $RESPONSE_SIZE 800
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 1600 $RESPONSE_SIZE 1600
#	echo "" >> throughput.csv
done

Rscript thput_aggr.R
sleep 1s

mv throughput.png "throughput_"$MODE"_"$NUMBER_CLIENTS"_"$NUMBER_SERVERS".png"
mv throughput.csv "throughput_"$MODE"_"$NUMBER_CLIENTS"_"$NUMBER_SERVERS".csv"

killall "athena_private"
killall "athena_gateway"
killall "ccnxVPN_Client"
killall "ccnxVPN_Server"
