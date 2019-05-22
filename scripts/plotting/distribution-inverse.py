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

from dumpAndPlot import *

def getHgroup(h):
    return round(h,1)

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

# dumphhstart(h_collection, min(resultDirs), True) 

# dumphhat(h_collection, min(resultDirs)) 

# dumphSamples(h_collection_sampleStates, min(resultDirs)) 

# plotHist(h_collection, min(resultDirs)) 

# plotScatter(all_h, all_hs, min(resultDirs)) 

# dump2csv(all_h, all_hs, min(resultDirs)) 

