
sk50 = read.csv("./public_key/throughput_pk_50_50.csv",sep=',')$rtt
pk50 = read.csv("./symm_key/throughput_sk_50_50.csv",sep=',')$rtt

sk100 = read.csv("./public_key/throughput_pk_100_100.csv",sep=',')$rtt
pk100 = read.csv("./symm_key/throughput_sk_100_100.csv",sep=',')$rtt

sk150 = read.csv("./public_key/throughput_pk_150_150.csv",sep=',')$rtt
pk150 = read.csv("./symm_key/throughput_sk_150_150.csv",sep=',')$rtt

sk200 = read.csv("./public_key/throughput_pk_200_200.csv",sep=',')$rtt
pk200 = read.csv("./symm_key/throughput_sk_200_200.csv",sep=',')$rtt

sk250 = read.csv("./public_key/throughput_pk_250_250.csv",sep=',')$rtt
pk250 = read.csv("./symm_key/throughput_sk_250_250.csv",sep=',')$rtt

sk300 = read.csv("./public_key/throughput_pk_300_300.csv",sep=',')$rtt
pk300 = read.csv("./symm_key/throughput_sk_300_300.csv",sep=',')$rtt

clients = c("50","100","150","200","250","300")

SYMM = c(mean(sk50),mean(sk100),mean(sk150),mean(sk200),mean(sk250),mean(sk300))    #seconds
PKE = c(mean(pk50),mean(pk100),mean(pk150),mean(pk200),mean(pk250),mean(pk300))     #seconds

errf <- function(array){

    object <- sd(array)/sqrt(length(array))

    return(object)
}

CI1 = 1.64*c(errf(sk50),errf(sk100),errf(sk150),errf(sk200),errf(sk250),errf(sk300))
CI2 = 1.64*c(errf(pk50),errf(pk100),errf(pk150),errf(pk200),errf(pk250),errf(pk300))

errs = c(rbind(CI1,CI2)) 
means = c(rbind(SYMM,PKE)) 
data <- data.frame(rbind(SYMM,PKE))

pdf("new_result_rtt.pdf")

barCenters <- barplot(as.matrix(data), ylab="Avg. RTT (s)",
    xlab="Number of simultaneous clients", col=c("blue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=clients,ylim=c(0,(max(means)+ max(errs))*1.2),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1.5)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)


dev.off()

psk50 = read.csv("./public_key/throughput_pk_50_50.csv",sep=',')$payload_size/(1024*1024)
ppk50 = read.csv("./symm_key/throughput_sk_50_50.csv",sep=',')$payload_size/(1024*1024)

psk100 = read.csv("./public_key/throughput_pk_100_100.csv",sep=',')$payload_size/(1024*1024)
ppk100 = read.csv("./symm_key/throughput_sk_100_100.csv",sep=',')$payload_size/(1024*1024)

psk150 = read.csv("./public_key/throughput_pk_150_150.csv",sep=',')$payload_size/(1024*1024)
ppk150 = read.csv("./symm_key/throughput_sk_150_150.csv",sep=',')$payload_size/(1024*1024)

psk200 = read.csv("./public_key/throughput_pk_200_200.csv",sep=',')$payload_size/(1024*1024)
ppk200 = read.csv("./symm_key/throughput_sk_200_200.csv",sep=',')$payload_size/(1024*1024)

psk250 = read.csv("./public_key/throughput_pk_250_250.csv",sep=',')$payload_size/(1024*1024)
ppk250 = read.csv("./symm_key/throughput_sk_250_250.csv",sep=',')$payload_size/(1024*1024)

psk300 = read.csv("./public_key/throughput_pk_300_300.csv",sep=',')$payload_size/(1024*1024)
ppk300 = read.csv("./symm_key/throughput_sk_300_300.csv",sep=',')$payload_size/(1024*1024)

data_SYMM = c(mean(psk50),mean(psk100),mean(psk150),mean(psk200),mean(psk250),mean(psk300))    #Mbits (Avg of 5 runs)
data_PKE = c(mean(ppk50),mean(ppk100),mean(ppk150),mean(ppk200),mean(ppk250),mean(ppk300))     #Mbits (Avg of 5 runs)


SYMM = data_SYMM/SYMM
PKE = data_PKE/PKE

errf <- function(array){

    object <- sd(array)/sqrt(length(array))

    return(object)
}

CI1 = 1.64*c(errf(psk50/sk50),errf(psk100/sk100),errf(psk150/sk150),errf(psk200/sk200),errf(psk250/sk250),errf(psk300/sk300))
CI2 = 1.64*c(errf(ppk50/pk50),errf(ppk100/pk100),errf(ppk150/pk150),errf(ppk200/pk200),errf(ppk250/pk250),errf(ppk300/pk300))

errs = c(rbind(CI1,CI2)) 
means = c(rbind(SYMM,PKE)) 
data <- data.frame(rbind(SYMM,PKE))

pdf("new_result_thput.pdf")

barCenters <- barplot(as.matrix(data), ylab="Avg. Download Speed Client [mbps]",
    xlab="Number of simultaneous clients", col=c("blue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=clients,ylim=c(0,(max(means)+ max(errs))*1.2),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1.5)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)


dev.off()


