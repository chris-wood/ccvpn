# Setup

## Standard
./gateway/ccnx/forwarder/athena/command-line/athena/athena_private ccnx:/gateway ../key.pub ../key.sec
./gateway/ccnx/forwarder/athena/command-line/athena/athena ccnx:/foo 1 ccnx:/producer tcp://localhost:9695/name=tunnel/local=false -c tcp://localhost:9696/listener/local=false
METIS_PORT=9695 ./ccnxVPN_Server -l ccnx:/producer
METIS_PORT=9696 ./ccnxVPN_Client -l ccnx:/producer -c 100 -f

## VPN

./gateway/ccnx/forwarder/athena/command-line/athena/athena_private ccnx:/gateway ../key.pub ../key.sec
./gateway/ccnx/forwarder/athena/command-line/athena/athena_gateway ccnx:/foo 1 ccnx:/producer ccnx:/gateway ../key.pub tcp://localhost:9695/name=tunnel/local=false -c tcp://localhost:9696/listener/local=false
METIS_PORT=9695 ./ccnxVPN_Server -l ccnx:/producer
METIS_PORT=9696 ./ccnxVPN_Client -l ccnx:/producer -c 100 -f

# Experiments

- n consumers and m producers
- \rho - packet send rate
- \gamma - response size 
- transport mechanism - TCP vs UDP vs Ethernet
