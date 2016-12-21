v = read.csv("throughput.csv",sep=',')
v$na=NULL
v = v*8.0/1000000
png("../paper/throughput_pk_1x1.png")
boxplot(v,xlab="Number of interests issued per consumer",ylab="Througput [mbps]", names=c(25,50,100,200,400,800,1600,3200), main="1 Producer X 1 Consumer")
grid(10,10)
dev.off()
