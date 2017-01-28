
v1 = read.csv("./public_key/throughput_pk_1_1.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_1_1.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_1_1.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
dev.off()

v1 = read.csv("./public_key/throughput_pk_2_1.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_2_1.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_2_1.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
legend('topright', c("Symmetric","PKE"), fill=c("darkblue", "red"), cex=1.5)
dev.off()

v1 = read.csv("./public_key/throughput_pk_3_1.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_3_1.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_3_1.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
dev.off()

v1 = read.csv("./public_key/throughput_pk_4_1.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_4_1.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_4_1.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
dev.off()

v1 = read.csv("./public_key/throughput_pk_2_2.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_2_2.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_2_2.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
legend('topright', c("Symmetric","PKE"), fill=c("darkblue", "red"), cex=1.5)
dev.off()

v1 = read.csv("./public_key/throughput_pk_3_3.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_3_3.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_3_3.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
dev.off()

v1 = read.csv("./public_key/throughput_pk_4_4.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_4_4.csv",sep=',')

a=aggregate(v1,by=list(v1$n_pkt),FUN=mean)
b=aggregate(v2,by=list(v2$n_pkt),FUN=mean)

png("throughput_4_4.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(b$thput)/1000000,names=b$n_pkt,xlab="Interest issuance rate per consumer per second",ylab="Data rate per consumer[mbps]", col="darkblue",ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(a$thput)/1000000, col="red",space=1.5,ylim=c(0,10+max(c(8*(b$thput)/1000000),8*(a$thput)/1000000)))
dev.off()
