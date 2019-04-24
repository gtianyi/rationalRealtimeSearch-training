import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
import re
from os import listdir
from collections import defaultdict
from collections import OrderedDict

def makeHistrogram(h, hs, fileDir):
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})
    ax = sns.distplot(np.array(hs), kde=False)
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    ylable = '%.2f' % h  
    plt.ylabel("h="+ylable, color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.savefig(fileDir+"h"+str(h).zfill(5)+".eps", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def dump2file(h, hs, outFile):
    #save to 0.1 to make histogramo
    hs = [round(v,2) for v in hs] # this is dangrous, but otherwise we have each data point a bin
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
    # ax = sns.jointplot(x="h",y="h*",data=df, kind = "hex")
    ax = sns.scatterplot(x="h",y="h*",data=df, alpha = 0.01)
    plt.ylabel("h*", color='black', fontsize=18)
    plt.xlabel("h", color='black', fontsize=18)
    plt.savefig(fileDir+"scatter-inverse.pdf", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def dumphhat(h, hs, outFile):
    outFile.write(str(h)+' '+str(sum(hs)/len(hs)))
    outFile.write("\n")

def dump2file_sample_states(h, samples, outFile):
    #save to 0.1 to make histogramo
    outFile.write(str(h)+' '+str(len(samples))+' ')
    for sample in samples:
        outFile.write(sample+' ')
    outFile.write("\n")



# Hard coded result directories
resultDirs = {"inverse_20_0.1_200"}

h_collection = defaultdict(list)
h_collection_sampleStates = defaultdict(list)
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
                line = line.split("lenght",1)[1] #this is to avoid error when mixed line for multi-process
                for s in line.split():
                    # print(file)
                    if any(c.isdigit() for c in s):
                        g = re.search(r"\d+(\.\d+)?", s) # to avoid number mmixed with end
                        s = g.group(0)
                        hs = float(s)

        if h!=999.0 and hs!=999.0:
            h_collection[getHgroup(h)].append(hs)
            h_collection_sampleStates[getHgroup(h)].append(file.split(".")[0]+".st")
            all_h.append(h)
            all_hs.append(hs)

print("h count "+str(len(h_collection))) 

print("plotting...")


# f = open("../../../results/SlidingTilePuzzle/sampleData/"+min(resultDirs)+"-statSummary.txt","w")
# f_hhat = open("../../../results/SlidingTilePuzzle/sampleData/"+min(resultDirs)+"-hhat.txt","w")
# plotDir ="../../../plots/hist/"+min(resultDirs)+"/"

# od = OrderedDict(sorted(h_collection.items()))

# for h, hslist in od.items():
    # dump2file(h,hslist,f)
    # dumphhat(h,hslist,f_hhat)
    # if len(hslist) > 0:
        # makeHistrogram(h,hslist,plotDir)


od2 = OrderedDict(sorted(h_collection_sampleStates.items()))

f_samples = open("../../../results/SlidingTilePuzzle/sampleData/"+min(resultDirs)+"-samples.txt","w")

for h, samples in od2.items():
    dump2file_sample_states(h,samples,f_samples)
 

# makeScatterPlot(all_h, all_hs, plotDir)
