######Total Transmission Time##################################################
tsk50 = read.csv("./public_key/throughput_pk_50_50.csv",sep=',')$total_time
tpk50 = read.csv("./symm_key/throughput_sk_50_50.csv",sep=',')$total_time

tsk100 = read.csv("./public_key/throughput_pk_100_100.csv",sep=',')$total_time
tpk100 = read.csv("./symm_key/throughput_sk_100_100.csv",sep=',')$total_time

tsk150 = read.csv("./public_key/throughput_pk_150_150.csv",sep=',')$total_time
tpk150 = read.csv("./symm_key/throughput_sk_150_150.csv",sep=',')$total_time

tsk200 = read.csv("./public_key/throughput_pk_200_200.csv",sep=',')$total_time
tpk200 = read.csv("./symm_key/throughput_sk_200_200.csv",sep=',')$total_time

tsk250 = read.csv("./public_key/throughput_pk_250_250.csv",sep=',')$total_time
tpk250 = read.csv("./symm_key/throughput_sk_250_250.csv",sep=',')$total_time

tsk300 = read.csv("./public_key/throughput_pk_300_300.csv",sep=',')$total_time
tpk300 = read.csv("./symm_key/throughput_sk_300_300.csv",sep=',')$total_time

################################################################################


######Received Payloads#########################################################
psk50 = read.csv("./public_key/throughput_pk_50_50.csv",sep=',')$payload_size
ppk50 = read.csv("./symm_key/throughput_sk_50_50.csv",sep=',')$payload_size

psk100 = read.csv("./public_key/throughput_pk_100_100.csv",sep=',')$payload_size
ppk100 = read.csv("./symm_key/throughput_sk_100_100.csv",sep=',')$payload_size

psk150 = read.csv("./public_key/throughput_pk_150_150.csv",sep=',')$payload_size
ppk150 = read.csv("./symm_key/throughput_sk_150_150.csv",sep=',')$payload_size

psk200 = read.csv("./public_key/throughput_pk_200_200.csv",sep=',')$payload_size
ppk200 = read.csv("./symm_key/throughput_sk_200_200.csv",sep=',')$payload_size

psk250 = read.csv("./public_key/throughput_pk_250_250.csv",sep=',')$payload_size
ppk250 = read.csv("./symm_key/throughput_sk_250_250.csv",sep=',')$payload_size

psk300 = read.csv("./public_key/throughput_pk_300_300.csv",sep=',')$payload_size
ppk300 = read.csv("./symm_key/throughput_sk_300_300.csv",sep=',')$payload_size
################################################################################


data_SYMM = c(sum(psk50),sum(psk100),sum(psk150),sum(psk200),sum(psk250),sum(psk300))/(1024*1024*5)    #Mbits (Avg of 5 runs)
data_PKE = c(sum(ppk50),sum(ppk100),sum(ppk150),sum(ppk200),sum(ppk250),sum(ppk300))/(1024*1024*5)     #Mbits (Avg of 5 runs)

time_SYMM = c(mean(tsk50),mean(tsk100),mean(tsk150),mean(tsk200),mean(tsk250),mean(tsk300))/(1000000)    #avg time in sec
time_PKE = c(mean(tpk50),mean(tpk100),mean(tpk150),mean(tpk200),mean(tpk250),mean(tpk300))/(1000000)     #avg time in sec

SYMM = data_SYMM/time_SYMM
PKE = data_PKE/time_PKE

clients = c("50","100","150","200","250","300")

means = c(rbind(SYMM,PKE)) 
data <- data.frame(rbind(SYMM,PKE))

pdf("thput_net.pdf")

barCenters <- barplot(as.matrix(data), ylab="Network Throughput (Mbps)",
    xlab="Number of simultaneous clients", col=c("blue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=clients,ylim=c(0,max(means)*1.2),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1.5)

dev.off()
