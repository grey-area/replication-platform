data <- read.table("time1.dat",sep="\t",header=TRUE)

pdf('loopy1.pdf')
ggplot(data, aes(x = t, y = loopy_mean)) +  
  geom_bar(position = position_dodge(), stat="identity", fill="blue") + 
  geom_errorbar(aes(ymin=loopy_mean-loopy_std, ymax=loopy_mean+loopy_std)) +
  ggtitle("Test") + 
  theme_bw() + 
  theme(panel.grid.major = element_blank())
dev.off()


