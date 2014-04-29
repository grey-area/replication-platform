'''
A program to take results from many runs and collates them
Takes time series and puts them together - calculates means, std. dev.s
'''

import sys, os
import numpy as np

def main():
    if len(sys.argv) < 2:
        sys.exit()
    else:
        data1 = {}
        data2 = {}
        mean1 = {}
        mean2 = {}
        stds1 = {}
        stds2 = {}
        perc5_1 = {}
        perc5_2 = {}
        perc95_1 = {}
        perc95_2 = {}
        perc25_1 = {}
        perc25_2 = {}
        perc50_1 = {}
        perc50_2 = {}
        perc75_1 = {}
        perc75_2 = {}
        times1 = []
        times2 = []

        experimentDirectory = os.getcwd() + "/results/" + sys.argv[1] + "/"
        first = True
        for path, subpath, files in os.walk(experimentDirectory):
            if "run" in subpath: # everything below here constitutes multiple runs of the same setup
                name = '?'
                if "unloopy" in path:
                    name = "unloopy"
                elif 'loopy' in path and 'half-decoder' not in path:
                    name = "loopy"
                elif 'loopy' in path:
                    name = "loopy-h"
                elif 'ga' in path:
                    name = "ga"

                data1[name] = []
                data2[name] = []
                mean1[name] = []
                mean2[name] = []
                stds1[name] = []
                stds2[name] = []
                perc5_1[name] = []
                perc25_1[name] = []
                perc50_1[name] = []
                perc75_1[name] = []
                perc95_1[name] = []
                perc5_2[name] = []
                perc25_2[name] = []
                perc50_2[name] = []
                perc75_2[name] = []
                perc95_2[name] = []

                for path2, subpath2, files2 in os.walk(path+'/run'):
                    if files2 != [] and "time1.dat" in files2 and "time2.dat" in files2:
                        thisData1 = []
                        thisData2 = []
                        with open(path2+"/time1.dat") as f:
                            lines = f.readlines()
                            for line in lines:
                                splitLine = line.replace('\n','').split('\t')
                                if first:
                                    times1.append(int(splitLine[0]))
                                thisData1.append(float(splitLine[1]))
                        data1[name].append(thisData1)
                        with open(path2+"/time2.dat") as f:
                            lines = f.readlines()
                            for line in lines:
                                splitLine = line.replace('\n','').split('\t')
                                if first:
                                    times2.append(int(splitLine[0]))
                                thisData2.append(float(splitLine[1]))
                        data2[name].append(thisData2)
                        first = False
                data1[name] = np.array(data1[name])
                data2[name] = np.array(data2[name])
                mean1[name] = data1[name].mean(axis=0)
                mean2[name] = data2[name].mean(axis=0)
                stds1[name] = data1[name].std(axis=0)
                stds2[name] = data2[name].std(axis=0)
                perc5_1[name]  = np.percentile(data1[name], 5, axis=0)
                perc5_2[name]  = np.percentile(data2[name], 5, axis=0)
                perc25_1[name] = np.percentile(data1[name], 25, axis=0)
                perc25_2[name] = np.percentile(data2[name], 25, axis=0)
                perc50_1[name] = np.percentile(data1[name], 50, axis=0)
                perc50_2[name] = np.percentile(data2[name], 50, axis=0)
                perc75_1[name] = np.percentile(data1[name], 75, axis=0)
                perc75_2[name] = np.percentile(data2[name], 75, axis=0)
                perc95_1[name] = np.percentile(data1[name], 95, axis=0)
                perc95_2[name] = np.percentile(data2[name], 95, axis=0)


        with open(experimentDirectory + "time1.dat",'w') as f:
            f.write("t")
            for name in mean1.keys():
                f.write('\t')
                f.write(name + '_mean')
                f.write('\t')
                f.write(name + '_std')
                f.write('\t')
                f.write(name + '_5')
                f.write('\t')
                f.write(name + '_25')
                f.write('\t')
                f.write(name + '_50')
                f.write('\t')
                f.write(name + '_75')
                f.write('\t')
                f.write(name + '_95')
            f.write('\n')
            for e,time in enumerate(times1):
                f.write(str(time))
                for name in mean1.keys():
                    f.write('\t')
                    f.write(str(mean1[name][e]))
                    f.write('\t')
                    f.write(str(stds1[name][e]))
                    f.write('\t')
                    f.write(str(perc5_1[name][e]))
                    f.write('\t')
                    f.write(str(perc25_1[name][e]))
                    f.write('\t')
                    f.write(str(perc50_1[name][e]))
                    f.write('\t')
                    f.write(str(perc75_1[name][e]))
                    f.write('\t')
                    f.write(str(perc95_1[name][e]))
                f.write('\n')
        with open(experimentDirectory + "time2.dat",'w') as f:
            f.write("t")
            for name in mean2.keys():
                f.write('\t')
                f.write(name + '_mean')
                f.write('\t')
                f.write(name + '_std')
                f.write('\t')
                f.write(name + '_5')
                f.write('\t')
                f.write(name + '_25')
                f.write('\t')
                f.write(name + '_50')
                f.write('\t')
                f.write(name + '_75')
                f.write('\t')
                f.write(name + '_95')
            f.write('\n')
            for e,time in enumerate(times2):
                f.write(str(time))
                for name in mean2.keys():
                    f.write('\t')
                    f.write(str(mean2[name][e]))
                    f.write('\t')
                    f.write(str(stds2[name][e]))
                    f.write('\t')
                    f.write(str(perc5_2[name][e]))
                    f.write('\t')
                    f.write(str(perc25_2[name][e]))
                    f.write('\t')
                    f.write(str(perc50_2[name][e]))
                    f.write('\t')
                    f.write(str(perc75_2[name][e]))
                    f.write('\t')
                    f.write(str(perc95_2[name][e]))
                f.write('\n')

        '''with open(experimentDirectory + "results.dat",'w') as f:
            first = True
            for v in variables:
                if first:
                    first = False
                else:
                    f.write('\t')
                f.write(v)
            f.write('\n')

            for datum in data:
                first = True
                for v in variables:
                    if first:
                        first = False
                    else:
                        f.write('\t')
                    if v in datum.keys():
                        f.write(datum[v])
                    else:
                        f.write("NULL")
                f.write('\n')'''
            


if __name__ == '__main__':
   main()
