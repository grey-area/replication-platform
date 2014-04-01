library(ggplot2)

data<-read.table("time2.dat",sep="\t",header=TRUE)

# Loopy
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50)) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.2) + theme_bw()
ggsave("plots/s_loopy.pdf")

# Unloopy
ggplot(data, aes(t)) + geom_line(aes(y=unloopy_50)) + geom_ribbon(aes(ymin=unloopy_25, ymax=unloopy_75), alpha=0.2) + theme_bw()
ggsave("plots/s_unloopy.pdf")

# GA
ggplot(data, aes(t)) + geom_line(aes(y=ga_50)) + geom_ribbon(aes(ymin=ga_25, ymax=ga_75), alpha=0.2) + theme_bw()
ggsave("plots/s_ga.pdf")

# Loopy_H
ggplot(data, aes(t)) + geom_line(aes(y=loopy.h_50)) + geom_ribbon(aes(ymin=loopy.h_25, ymax=loopy.h_75), alpha=0.2) + theme_bw()
ggsave("plots/s_loopy-h.pdf")

# Loopy vs unloopy
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50)) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.2) + geom_line(aes(y=unloopy_50)) + geom_ribbon(aes(ymin=unloopy_25, ymax=unloopy_75), alpha=0.2) + theme_bw()
ggsave("plots/loopy_vs_unloopy.pdf")

# Loopy vs GA
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50)) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.2) + geom_line(aes(y=ga_50)) + geom_ribbon(aes(ymin=ga_25, ymax=ga_75), alpha=0.2) + theme_bw()
ggsave("plots/loopy_vs_ga.pdf")

# Loopy vs loopy-h
ggplot(data, aes(t)) + geom_line(aes(y=loopy_50)) + geom_ribbon(aes(ymin=loopy_25, ymax=loopy_75), alpha=0.2) + geom_line(aes(y=loopy.h_50)) + geom_ribbon(aes(ymin=loopy.h_25, ymax=loopy.h_75), alpha=0.2) + theme_bw()
ggsave("plots/loopy_vs_loopy-h.pdf")

# Loopy-h vs unloopy
ggplot(data, aes(t)) + geom_line(aes(y=loopy.h_50)) + geom_ribbon(aes(ymin=loopy.h_25, ymax=loopy.h_75), alpha=0.2) + geom_line(aes(y=unloopy_50)) + geom_ribbon(aes(ymin=unloopy_25, ymax=unloopy_75), alpha=0.2) + theme_bw()
ggsave("plots/loopy-h_vs_unloopy.pdf")

