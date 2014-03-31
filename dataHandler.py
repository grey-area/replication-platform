'''
A program to take results from many runs and collates them
Takes independent variables from config file, and dependent variables from data file
'''

import sys, os
from collections import OrderedDict # using it as an ordered set

def readData(fileName, runDict):
    with open(fileName) as f:
        lines = f.readlines()
        for line in lines:
            if '=' in line:
                splitLine = line.replace(" ","").replace("\n","").split('=')
                runDict[splitLine[0]] = splitLine[1]

def main():
    if len(sys.argv) < 2:
        sys.exit()
    else:
        variables = OrderedDict() # Set of all options
        data = []       # Data

        experimentDirectory = os.getcwd() + "/results/" + sys.argv[1] + "/"
        for path, subpath, files in os.walk(experimentDirectory):
            if files != [] and 'config.cfg' in files and 'data.dat' in files:
                runDict = {}

                # Read independent variables from path/config.cfg
                readData(path + "/config.cfg", runDict)

                # Read dependent variables from path/data.dat
                readData(path + "/data.dat", runDict)

                for x in runDict.keys():
                    variables[x] = True
                data.append(runDict)

        with open(experimentDirectory + "results.dat",'w') as f:
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
                f.write('\n')
            


if __name__ == '__main__':
   main()
