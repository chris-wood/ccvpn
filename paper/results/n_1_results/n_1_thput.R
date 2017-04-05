######Total Transmission Time##################################################
errf <- function(array){

    object <- 1.64*sd(array)/sqrt(length(array))

    return(object)
}

files = list.files("./symm_key/")

RTT_SYMM = c()
ERR_RTT_SYMM = c()
SYMM = c()
ERR_SYMM = c()

for (file in files){
  print(file)
  tsk = read.csv(paste("./symm_key/",file,sep=""),sep=',')$total_time/1000000   #seconds
  dsk = read.csv(paste("./symm_key/",file,sep=""),sep=',')$total_data/(1024*1024) #Mbits
  rttsk = read.csv(paste("./symm_key/",file,sep=""),sep=',')$rtt #s
  thput = mean(dsk/tsk)
  err_thput = errf(dsk/tsk)
  rtt = mean(rttsk)
  err_rtt = errf(rttsk)

  SYMM = c(SYMM,thput)
  ERR_SYMM = c(ERR_SYMM,err_thput)
  RTT_SYMM = c(RTT_SYMM,rtt)
  ERR_RTT_SYMM = c(ERR_RTT_SYMM,err_rtt)
}


files = list.files("./public_key/")

RTT_PKE = c()
ERR_RTT_PKE = c()
PKE = c()
ERR_PKE = c()
for (file in files){
  print(file)
  tsk = read.csv(paste("./public_key/",file,sep=""),sep=',')$total_time/1000000   #seconds
  dsk = read.csv(paste("./public_key/",file,sep=""),sep=',')$total_data/(1024*1024) #Mbits
  rttsk = read.csv(paste("./public_key/",file,sep=""),sep=',')$rtt #s
  thput = mean(dsk/tsk)
  err_thput = errf(dsk/tsk)
  rtt = mean(rttsk)
  err_rtt = errf(rttsk)

  PKE = c(PKE,thput)
  ERR_PKE = c(ERR_PKE,err_thput)
  RTT_PKE = c(RTT_PKE,rtt)
  ERR_RTT_PKE = c(ERR_RTT_PKE,err_rtt)

}

errs = c(rbind(ERR_RTT_SYMM,ERR_RTT_PKE)) 
means = c(rbind(RTT_SYMM,RTT_PKE)) 
data <- data.frame(rbind(RTT_SYMM,RTT_PKE))

ips = c()
increase = 10
for (x in 1:length(files)){
    ips = c(ips,increase*x)
}


pdf("n_1_rtt.pdf")
barCenters <- barplot(as.matrix(data), ylab="Avg. RTT [s]",
    xlab="Simultaneous clients",main="Content packet size = 10KBytes", col=c("lightblue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=ips,ylim=c(0,(max(means)+ max(errs))*1.2),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1)

grid(nx=NA, ny=NULL, lwd=1, lty=2)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)


dev.off()


png("n_1_rtt.png")
barCenters <- barplot(as.matrix(data), ylab="Avg. RTT [s]",
    xlab="Simultaneous clients",main="Content packet size = 10KBytes", col=c("lightblue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=ips,ylim=c(0,(max(means)+ max(errs))*1.2),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1)

grid(nx=NA, ny=NULL, lwd=1, lty=2)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)


dev.off()





errs = c(rbind(ERR_SYMM,ERR_PKE)) 
means = c(rbind(SYMM,PKE)) 
data <- data.frame(rbind(SYMM,PKE))

ips = c()
increase = 10
for (x in 1:length(files)){
    ips = c(ips,increase*x)
}


pdf("n_1_thput.pdf")
barCenters <- barplot(as.matrix(data), ylab="Throughput per client [mbps]",
    xlab="Simultaneous clients",main="Content packet size = 10KBytes", col=c("lightblue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=ips,ylim=c(0,(max(means)+ max(errs))*1.1),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1)

grid(nx=NA, ny=NULL, lwd=1, lty=2)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)


dev.off()


png("n_1_thput.png")
barCenters <- barplot(as.matrix(data), ylab="Throughput per client [mbps]",
    xlab="Simultaneous clients",main="Content packet size = 10KBytes", col=c("lightblue","red"),
 	legend = c("SYMM","PKE"), beside=TRUE, names.arg=ips,ylim=c(0,(max(means)+ max(errs))*1.1),
    cex.lab=1.5, cex.axis=1.5, cex.main=1.5, cex.sub=1.5,, cex.names=1)

grid(nx=NA, ny=NULL, lwd=1, lty=2)

arrows(barCenters, means - errs, barCenters,
       means + errs, lwd = 1.5, angle = 90,
       code = 3, length = 0.05)

dev.off()
