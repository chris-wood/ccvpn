v = read.csv("throughput.csv",sep=',')
v$na=NULL
v = v*8.0/1000000
png("throughput.png")
boxplot(v,xlab="Number of interests issued per consumer",ylab="Througput(mbps)", names=c(25,50,100,200,400,800,1600,3200))
dev.off()
