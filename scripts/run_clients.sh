#!/usr/bin/bash

chmod +w times.csv

echo "intReg,intEncap,intDecap,contReg,contEnc,contDec" > times.csv

NUMBER_CLIENTS=$1
NUMBER_SERVERS=$2
RESPONSE_SIZE=$3
MODE=$4

echo "rtt,thput,total_time,total_data" > throughput.csv
echo "n_pkt,dropped" > dropped.csv

for i in `seq 1 2`;
do
	echo "\nround $i $RESPONSE_SIZE $NUMBER_CLIENTS\n"
    sleep 2s
	sh start_consumers_same_server.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 13 $RESPONSE_SIZE 104
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 25 $RESPONSE_SIZE 25
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 50 $RESPONSE_SIZE 50
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 100 $RESPONSE_SIZE 100
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 200 $RESPONSE_SIZE 200
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 400 $RESPONSE_SIZE 400
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 800 $RESPONSE_SIZE 800
#	sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS 1600 $RESPONSE_SIZE 1600
    sh experiments_clean.sh
    sleep 2s
    sh setup_network.sh $NUMBER_SERVERS $MODE &
    sleep 10s
done

#Rscript thput_aggr.R
sleep 1s

#mv throughput.png "throughput_"$MODE"_"$NUMBER_CLIENTS"_"$NUMBER_SERVERS".png"
mv throughput.csv $NUMBER_CLIENTS"_"$NUMBER_SERVERS"_throughput_"$MODE"_.csv"

rm dropped*.csv
#mv dropped.png "dropped_"$MODE"_"$NUMBER_CLIENTS"_"$NUMBER_SERVERS".png"
#mv dropped.csv "dropped_"$MODE"_"$NUMBER_CLIENTS"_"$NUMBER_SERVERS".csv"
