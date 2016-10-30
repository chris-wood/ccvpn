# ccvpn

Note: test.p12 keystore password is "test"

- Start:

./gateway/ccnx/forwarder/athena/command-line/athena/athena -c tcp://localhost:9695/listener

./gateway/ccnx/forwarder/athena/command-line/athena/athena -c tcp://localhost:9696/listener

- Setup the link:

./gateway/ccnx/forwarder/athena/command-line/athenactl/athenactl -f ../test.p12 -p test -a tcp://localhost:9695 add link tcp://localhost:9696/name=test

- Add the route/key combo

./gateway/ccnx/forwarder/athena/command-line/athenactl/athenactl -f ../test.p12 -p test -a tcp://localhost:9695 add route test ccnx:/hello 5 FFFFFFasdasda

