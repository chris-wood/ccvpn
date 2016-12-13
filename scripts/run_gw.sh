../b/gateway/ccnx/forwarder/athena/command-line/athena/athena_private ccnx:/gateway ../key.pub ../key.sec &
../b/gateway/ccnx/forwarder/athena/command-line/athena/athena_gateway ccnx:/foo 1 ccnx:/producer ccnx:/gateway ../key.pub tcp://localhost:9695/name=tunnel/local=false -c tcp://localhost:9696/listener/local=false &
sh start_producers.sh ../b/ccnxVPN_Server ccnx:/producer 3 &





export METIS_PORT=9695
../b/ccnxVPN_Server -l ccnx:/producer
export METIS_PORT=9696
../b/ccnxVPN_Client -l ccnx:/producer -c 100 -f

