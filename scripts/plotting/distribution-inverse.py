import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
from os import listdir
from collections import defaultdict

def makeHistrogram(h, hs, fileDir):
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.distplot(np.array(hs), kde=False)
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    ylable = '%.1f' % h  
    plt.ylabel("h="+ylable, color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.savefig(fileDir+"h"+str(h).zfill(4)+".eps", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def dump2file(h, hs, outFile):
    #save to 0.1 to make histogramo
    hs = [round(v,1) for v in hs]
    hsSet = set(hs)
    outFile.write(str(h)+' '+str(len(hs))+' ')
    for hsvalue in hsSet:
        valueCount = hs.count(hsvalue)
        outFile.write(str(hsvalue)+' '+str(valueCount)+' ')
    outFile.write("\n")

def getHgroup(h):
    return round(h,1)

def makeScatterPlot(h, hs,fileDir):
    df = pd.DataFrame({"h":h,"h*":hs})
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.scatterplot(x="h",y="h*",data=df, alpha=0.5)
    plt.ylabel("h*", color='black', fontsize=18)
    plt.xlabel("h", color='black', fontsize=18)
    plt.savefig(fileDir+"scatter-inverse.pdf", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

# Hard coded result directories
resultDirs = {"inverse"}

h_collection = defaultdict(list)
all_h = []
all_hs = []

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../../../results/SlidingTilePuzzle/sampleData/" + dir):
        f = open("../../../results/SlidingTilePuzzle/sampleData/" + dir + "/" + file, "r")

        h = float(999)
        hs = float(999.0)

        for line in f:
            line = line.replace('"','')

            if "initial heuristic" in line:
                for s in line.split():
                    if any(c.isdigit() for c in s):
                        h = float(s)

            # if "solution length" in line:
            if "solution lenght" in line:
                for s in line.split():
                    if any(c.isdigit() for c in s):
                        hs = float(s)

        if h!=999.0 and hs!=999.0:
            h_collection[getHgroup(h)].append(hs)
            all_h.append(h)
            all_hs.append(hs)

print("h count "+str(len(h_collection))) 

print("plotting...")


# f = open("../../../results/SlidingTilePuzzle/sampleData/"+min(resultDirs)+"-statSummary.txt","w")
plotDir ="../../../plots/hist/"+min(resultDirs)+"/"

# for h, hslist in h_collection.items():
    # dump2file(h,hslist,f)
    # if len(hslist) > 0:
        # makeHistrogram(h,hslist,plotDir)

makeScatterPlot(all_h, all_hs, plotDir)
