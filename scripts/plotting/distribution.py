import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
from os import listdir
from collections import defaultdict

from dumpAndPlot import *

# Hard coded result directories
resultDirs = {"uniform"}

h_collection = defaultdict(list)
h_collection_sampleStates = defaultdict(list)

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

        if h!=999 and hs!=999:
            h_collection[h].append(hs)
            h_collection_sampleStates[h].append(file.split(".")[0]+".st")

# dumphhstart(h_collection, min(resultDirs)) 

dumphSamples(h_collection_sampleStates,min(resultDirs)) 

# plotHist(h_collection, min(resultDirs)) 
