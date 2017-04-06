files = list.files("./time_files")

names = c()
for (file in files){
  print(file)
  name = paste("./time_files/",file,sep="")
  names = c(names,name)
}

d1 = read.csv(names[1],head=F)$V1
d2 = read.csv(names[2],head=F)$V1
d3 = read.csv(names[3],head=F)$V1
d4 = read.csv(names[4],head=F)$V1
d5 = read.csv(names[5],head=F)$V1
d6 = read.csv(names[6],head=F)$V1

pdf("times.pdf",height=5,width=10)
boxplot(d1,d2,d3,d4,d5,d6,
        names= c("Alg.1 (PKE)","Alg.2 (PKE)","Alg.1 (SYMM)","Alg.2 (SYMM)","Alg.3","Alg.4"),
        outline=F,
        ylab = "Processing time [us]",
        xlab = "Algorithms of CCPVN Design")
grid(nx=NA, ny=NULL, lwd=1, lty=2)

dev.off()
