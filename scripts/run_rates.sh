#!/usr/bin/bash

chmod +w times.csv

echo "intReg,intEncap,intDecap,contReg,contEnc,contDec" > times.csv

NUMBER_CLIENTS=$1
NUMBER_SERVERS=$2
RESPONSE_SIZE=$3
MODE=$4

for j in `seq 0 19`;
do
    RATE=$((300 + 300 * j))
    echo "rtt,thput,total_time,total_data" > throughput.csv
    echo "n_pkt,dropped" > dropped.csv

    echo "Rate:"
    echo $RATE "pkts per s"
    for i in `seq 1 16`;
    do
	    echo "\nround $i\n"
        sleep 2s
	    sh start_consumers.sh ../b/ccnxVPN_Client ccnx:/producer $NUMBER_CLIENTS $RATE $RESPONSE_SIZE 6000

    done

    mv throughput.csv $RATE"_throughput_"$MODE"_.csv"

    #mv dropped.csv $RATE"_dropped_"$MODE"_.csv"
    rm dropped.csv
    rm times.csv

    sh experiments_clean.sh
    sleep 2s
    sh setup_network.sh 1 $MODE &
    sleep 10s

done
#Rscript thput_aggr.R
sleep 1s

