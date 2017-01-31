
v1 = read.csv("./10240/public_key/throughput_pk_50_50.csv",sep=',')
v2 = read.csv("./10240/symm_key/throughput_sk_50_50.csv",sep=',')

v10 = read.csv("./10240/public_key/throughput_pk_100_100.csv",sep=',')
v20 = read.csv("./10240/symm_key/throughput_sk_100_100.csv",sep=',')

v100 = read.csv("./10240/public_key/throughput_pk_150_150.csv",sep=',')
v200 = read.csv("./10240/symm_key/throughput_sk_150_150.csv",sep=',')

v1000 = read.csv("./10240/public_key/throughput_pk_200_200.csv",sep=',')
v2000 = read.csv("./10240/symm_key/throughput_sk_200_200.csv",sep=',')

v10000 = read.csv("./10240/public_key/throughput_pk_250_250.csv",sep=',')
v20000 = read.csv("./10240/symm_key/throughput_sk_250_250.csv",sep=',')

v100000 = read.csv("./10240/public_key/throughput_pk_300_300.csv",sep=',')
v200000 = read.csv("./10240/symm_key/throughput_sk_300_300.csv",sep=',')

y1 = c((8*mean(v1$thput)),(8*mean(v10$thput)),(8*mean(v100$thput)),(8*mean(v1000$thput)),(8*mean(v10000$thput)),(8*mean(v100000$thput)))

y2 = c((8*mean(v2$thput)),(8*mean(v20$thput)),(8*mean(v200$thput)),(8*mean(v2000$thput)),(8*mean(v20000$thput)),(8*mean(v200000$thput)))

png("10240_many_throughput.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,y1/1000000,names=c("50","100","150","200","250","300"),xlab="Number of consumers",ylab="Avg. data rate received per consumer [mbps]", col="darkgreen", ylim=c(0,20), space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,y2/1000000, col="yellow",space=1.5, ylim=c(0,20))
legend('topright', c("Symmetric","PKE"), fill=c("darkgreen", "yellow"), cex=1.5)
dev.off()


