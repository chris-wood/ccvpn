#!/usr/bin/bash

echo "\n\nUsage: sh run_clients.sh <NUMBER_CLIENTS> <PACKET_RATE> <RESPONSE_SIZE> <NUMBER_PACKETS>\n"
echo "setup_network.sh must be running with the same <NUMBER_PRODUCERS> parameter\n\n"

echo "intReg,intEncap,intDecap,contReg,contEnc,contDec" > times.csv

chmod +w times.csv

NUMBER_CLIENTS=$1
PACKET_RATE=$2
RESPONSE_SIZE=$3
NUMBER_PACKETS=$4
MULT=10

echo "25,50,100,200,400,800,1600,3200" > throughput.csv
for i in `seq 1 100`;
do
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 25
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 50
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 100
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 200
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 400
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 800
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 1600
	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $PACKET_RATE $RESPONSE_SIZE 3200
	echo "" >> throughput.csv
done

