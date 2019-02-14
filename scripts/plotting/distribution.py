import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
from os import listdir
from collections import defaultdict

def makeHistrogram(h, hs):
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.distplot(np.array(hs))
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    plt.ylabel("h="+str(h), color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.savefig("../../../plots/hist/h"+str(h).zfill(2)+".eps", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def dump2file(h, hs, outFile):
    hsSet = set(hs)
    outFile.write(str(h)+' '+str(len(hs))+' ')
    for hsvalue in hsSet:
        valueCount = hs.count(hsvalue)
        outFile.write(str(hsvalue)+' '+str(valueCount)+' ')
    outFile.write("\n")

# Hard coded result directories
resultDirs = {"w2"}

h_collection = defaultdict(list)

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../../../results/SlidingTilePuzzle/sampleData/" + dir):
        f = open("../../../results/SlidingTilePuzzle/sampleData/" + dir + "/" + file, "r")

        h = 999
        hs = 999

        for line in f:
            line = line.replace('"','')

            if "initial heuristic" in line:
                for s in line.split():
                    if s.isdigit():
                        h = int(s)

            if "solution length" in line:
                for s in line.split():
                    if s.isdigit():
                        hs = int(s)

        h_collection[h].append(hs)

print("h count "+str(len(h_collection))) 

print("plotting...")


f = open("../../../results/SlidingTilePuzzle/sampleData/"+min(resultDirs)+"-statSummary.txt","w")

for h, hslist in h_collection.items():
    dump2file(h,hslist,f)
    if len(hslist) == 200:
        makeHistrogram(h,hslist)
