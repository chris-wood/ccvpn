v = read.csv("throughput.csv",sep=',')
a=aggregate(v,by=list(v$n_pkt),FUN=mean)
png("throughput.png")
barplot(8*(a$thput)/1000000,names=a$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]")
grid(10,10)
dev.off()


v = read.csv("dropped.csv",sep=',')
a=aggregate(v,by=list(v$n_pkt),FUN=mean)
png("dropped.png")
barplot(100*(a$dropped),names=a$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Dropped packets (%)")
grid(10,10)
dev.off()
