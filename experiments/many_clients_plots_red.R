
v1 = read.csv("./public_key/throughput_pk_200_1.csv",sep=',')
v2 = read.csv("./symm_key/throughput_sk_200_1.csv",sep=',')

v10 = read.csv("./public_key/throughput_pk_400_1.csv",sep=',')
v20 = read.csv("./symm_key/throughput_sk_400_1.csv",sep=',')

v100 = read.csv("./public_key/throughput_pk_600_1.csv",sep=',')
v200 = read.csv("./symm_key/throughput_sk_600_1.csv",sep=',')

v1000 = read.csv("./public_key/throughput_pk_800_1.csv",sep=',')
v2000 = read.csv("./symm_key/throughput_sk_800_1.csv",sep=',')

v10000 = read.csv("./public_key/throughput_pk_1000_1.csv",sep=',')
v20000 = read.csv("./symm_key/throughput_sk_1000_1.csv",sep=',')

v100000 = read.csv("./public_key/throughput_pk_1200_1.csv",sep=',')
v200000 = read.csv("./symm_key/throughput_sk_1200_1.csv",sep=',')

y1 = c((length(v1$thput)/5)*mean(v1$thput),(length(v10$thput)/5)*mean(v10$thput),(length(v100$thput)/5)*mean(v100$thput),(length(v1000$thput)/5)*mean(v1000$thput),(length(v10000$thput)/5)*mean(v10000$thput),(length(v100000$thput)/5)*mean(v100000$thput))

y2 = c((length(v2$thput)/5)*mean(v2$thput),(length(v20$thput)/5)*mean(v20$thput),(length(v200$thput)/5)*mean(v200$thput),(length(v2000$thput)/5)*mean(v2000$thput),(length(v20000$thput)/5)*mean(v20000$thput),(length(v200000$thput)/5)*mean(v200000$thput))


png("many_throughput.png")
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(y1)/1000000,names=c("10","50","100","200","400","800"),xlab="Number of consumers",ylab="Sum of total data rate received by all consumers [mbps]", col="darkblue",ylim=c(0,10+max(8*(y1)/1000000,8*(y2)/1000000)),space=1)
par(new = TRUE)
barplot(cex.lab=1.5, cex.axis=1.5, cex.main=2.0, cex.sub=2.0,8*(y2)/1000000, col="red",space=1.5,ylim=c(0,10+max(8*(y1)/1000000,8*(y2)/1000000)))
legend('topleft', c("Symmetric","PKE"), fill=c("darkblue", "red"), cex=1.5)
dev.off()


