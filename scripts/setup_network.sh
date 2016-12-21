#!/usr/bin/bash

rm interest.csv
rm content.csv

NUMBER_PRODUCERS=$1

echo "\n\nUsage: sh setup_network.sh <NUMBER_PRODUCERS>\n\n"

../b/gateway/ccnx/forwarder/athena/command-line/athena/athena_private ccnx:/gateway /tmp/key.pub /tmp/key.sec &
../b/gateway/ccnx/forwarder/athena/command-line/athena/athena_gateway ccnx:/foo 1 ccnx:/producer ccnx:/gateway /tmp/key.pub tcp://localhost:9695/name=tunnel/local=false -c tcp://localhost:9696/listener/local=false &
sleep 1s
sh start_producers.sh ../b/ccnxVPN_Server ccnx:/producer $NUMBER_PRODUCERS

killall "athena_private"
killall "athena_gateway"
killall "ccnxVPN_Client"
