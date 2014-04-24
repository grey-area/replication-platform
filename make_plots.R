library(ggplot2)
library(methods)

data<-read.table("time2.dat",sep="\t",header=TRUE)

# Loopy
#ggplot(data, aes(t)) + geom_line(aes(y=loopy_50), colour="blue") + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.2) + theme_bw()
#ggsave("plots/s_loopy.pdf")

# Unloopy
#ggplot(data, aes(t)) + geom_line(aes(y=unloopy_50)) + geom_ribbon(aes(ymin=unloopy_25, ymax=unloopy_75), alpha=0.2) + theme_bw()
#ggsave("plots/s_unloopy.pdf")

# GA
#ggplot(data, aes(t)) + geom_line(aes(y=ga_50)) + geom_ribbon(aes(ymin=ga_25, ymax=ga_75), alpha=0.2) + theme_bw()
#ggsave("plots/s_ga.pdf")

# Loopy_H
#ggplot(data, aes(t)) + geom_line(aes(y=loopy.h_50)) + geom_ribbon(aes(ymin=loopy.h_25, ymax=loopy.h_75), alpha=0.2) + theme_bw()
#ggsave("plots/s_loopy-h.pdf")

#Couldn't get it to do the actual colours I wanted, and it was putting the names of the colours in the key
# Loopy vs unloopy
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50, colour="Meta-decoder", linetype="Meta-decoder")) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.15) + geom_line(aes(y=unloopy_50, colour="Solution decoder", linetype="Solution decoder")) + theme_bw() + geom_ribbon(aes(ymin=unloopy_25, ymax=unloopy_75), alpha=0.15) + scale_linetype_discrete(name = "") + scale_colour_discrete(name = "") + labs(x="Time (# fitness function evaluations)", y="Fitness (average)") + theme(legend.position=c(0.8,0.1))
ggsave("plots/loopy_vs_unloopy.pdf", width=6, height=5) 

# Loopy vs GA
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50,  colour="Meta-decoder", linetype="Meta-decoder")) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.15) + geom_line(aes(y=ga_50, , colour="Genetic algorithm", linetype="Genetic algorithm")) + geom_ribbon(aes(ymin=ga_25, ymax=ga_75), alpha=0.15) + theme_bw()+ scale_linetype_discrete(name = "") + scale_colour_discrete(name = "") + labs(x="Time (# fitness function evaluations)", y="Fitness (average)") + theme(legend.position=c(0.8,0.1))
ggsave("plots/loopy_vs_ga.pdf", width=6, height=5) 

# Loopy vs loopy-h
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50,  colour="Meta-decoder", linetype="Meta-decoder")) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.15) + geom_line(aes(y=loopy.h_50, colour="Meta-decoder (v2)", linetype="Meta-decoder (v2)")) + geom_ribbon(aes(ymin=loopy.h_25, ymax=loopy.h_75), alpha=0.15) + theme_bw()+ scale_linetype_discrete(name = "") + scale_colour_discrete(name = "") + labs(x="Time (# fitness function evaluations)", y="Fitness (average)") + theme(legend.position=c(0.8,0.1))
ggsave("plots/loopy_vs_loopy-h.pdf", width=6, height=5) 

# Loopy-h vs unloopy
ggplot(data, aes(t)) + geom_line(aes(y=loopy.h_50, colour="Meta-decoder (v2)", linetype="Meta-decoder (v2)")) + geom_ribbon(aes(ymin=loopy.h_25, ymax=loopy.h_75), alpha=0.15) + geom_line(aes(y=unloopy_50, colour="Solution decoder", linetype="Solution decoder")) + geom_ribbon(aes(ymin=unloopy_25, ymax=unloopy_75), alpha=0.15) + theme_bw()+ scale_linetype_discrete(name = "") + scale_colour_discrete(name = "") + labs(x="Time (# fitness function evaluations)", y="Fitness (average)") + theme(legend.position=c(0.8,0.1))
ggsave("plots/loopy-h_vs_unloopy.pdf", width=6, height=5) 

