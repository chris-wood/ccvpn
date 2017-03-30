
sk50 = read.csv("./public_key/throughput_pk_50_50.csv",sep=',')$thput
pk50 = read.csv("./symm_key/throughput_sk_50_50.csv",sep=',')$thput

sk100 = read.csv("./public_key/throughput_pk_100_100.csv",sep=',')$thput
pk100 = read.csv("./symm_key/throughput_sk_100_100.csv",sep=',')$thput

sk150 = read.csv("./public_key/throughput_pk_150_150.csv",sep=',')$thput
pk150 = read.csv("./symm_key/throughput_sk_150_150.csv",sep=',')$thput

sk200 = read.csv("./public_key/throughput_pk_200_200.csv",sep=',')$thput
pk200 = read.csv("./symm_key/throughput_sk_200_200.csv",sep=',')$thput

sk250 = read.csv("./public_key/throughput_pk_250_250.csv",sep=',')$thput
pk250 = read.csv("./symm_key/throughput_sk_250_250.csv",sep=',')$thput

sk300 = read.csv("./public_key/throughput_pk_300_300.csv",sep=',')$thput
pk300 = read.csv("./symm_key/throughput_sk_300_300.csv",sep=',')$thput

clients = c("50","100","150","200","250","300")

SYMM = c(mean(sk50),mean(sk100),mean(sk150),mean(sk200),mean(sk250),mean(sk300))/(1024*1024)    #Mbps
PKE = c(mean(pk50),mean(pk100),mean(pk150),mean(pk200),mean(pk250),mean(pk300))/(1024*1024)     #Mbps

errf <- function(array){

    object <- sd(array)/sqrt(length(array))

    return(object)
}

CI1 = 1.64*c(errf(sk50),errf(sk100),errf(sk150),errf(sk200),errf(sk250),errf(sk300))/(1024*1024)
CI2 = 1.64*c(errf(pk50),errf(pk100),errf(pk150),errf(pk200),errf(pk250),errf(pk300))/(1024*1024)

errs = c(rbind(CI1,CI2)) 
means = c(rbind(SYMM,PKE)) 
data <- data.frame(rbind(SYMM,PKE))

pdf("new_result_thput.pdf")

barCenters <- barplot(as.matrix(data), ylab="Throughput per client (Mbps)",
    xlab="Number of simultaneous clients", col=c("blue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=clients,ylim=c(0,(max(means)+ max(errs))*1.2),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1.5)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)
dev.off()


sk50 = read.csv("./public_key/throughput_pk_50_50.csv",sep=',')$thput
pk50 = read.csv("./symm_key/throughput_sk_50_50.csv",sep=',')$thput

sk100 = read.csv("./public_key/throughput_pk_100_100.csv",sep=',')$thput
pk100 = read.csv("./symm_key/throughput_sk_100_100.csv",sep=',')$thput

sk150 = read.csv("./public_key/throughput_pk_150_150.csv",sep=',')$thput
pk150 = read.csv("./symm_key/throughput_sk_150_150.csv",sep=',')$thput

sk200 = read.csv("./public_key/throughput_pk_200_200.csv",sep=',')$thput
pk200 = read.csv("./symm_key/throughput_sk_200_200.csv",sep=',')$thput

sk250 = read.csv("./public_key/throughput_pk_250_250.csv",sep=',')$thput
pk250 = read.csv("./symm_key/throughput_sk_250_250.csv",sep=',')$thput

sk300 = read.csv("./public_key/throughput_pk_300_300.csv",sep=',')$thput
pk300 = read.csv("./symm_key/throughput_sk_300_300.csv",sep=',')$thput

