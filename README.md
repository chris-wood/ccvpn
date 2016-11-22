# ccvpn

Note: test.p12 keystore password is "test"

- Start:

./gateway/ccnx/forwarder/athena/command-line/athena/athena -c tcp://localhost:9695/listener

./gateway/ccnx/forwarder/athena/command-line/athena/athena -c tcp://localhost:9696/listener

- Setup the link:

./gateway/ccnx/forwarder/athena/command-line/athenactl/athenactl -f ../test.p12 -p test -a tcp://localhost:9695 add link tcp://localhost:9696/name=test

- Add the route/key combo

./gateway/ccnx/forwarder/athena/command-line/athenactl/athenactl -f ../test.p12 -p test -a tcp://localhost:9695 add route test ccnx:/hello 5 FFFFFFasdasda

# REGULAR SETUP
./gateway/ccnx/forwarder/athena/command-line/athena/athena_private ccnx:/gateway ../key.pub ../key.sec
./gateway/ccnx/forwarder/athena/command-line/athena/athena ccnx:/foo 1 ccnx:/producer tcp://localhost:9695/name=tunnel/local=false -c tcp://localhost:9696/listener/local=false
METIS_PORT=9695 ./ccnxVPN_Server -l ccnx:/producer
METIS_PORT=9696 ./ccnxVPN_Client -l ccnx:/producer -c 100 -f


# VPN SETUP

./gateway/ccnx/forwarder/athena/command-line/athena/athena_private ccnx:/gateway ../key.pub ../key.sec
./gateway/ccnx/forwarder/athena/command-line/athena/athena_gateway ccnx:/foo 1 ccnx:/producer ccnx:/gateway ../key.pub tcp://localhost:9695/name=tunnel/local=false -c tcp://localhost:9696/listener/local=false
METIS_PORT=9695 ./ccnxVPN_Server -l ccnx:/producer
METIS_PORT=9696 ./ccnxVPN_Client -l ccnx:/producer -c 100 -f
