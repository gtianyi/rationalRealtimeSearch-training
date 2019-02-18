import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
from os import listdir
from collections import defaultdict

def dump2file(unsolved, outFile):
    outFile.write('unsolved: '+str(len(unsolved))+'\n')
    for filename in unsolved:
        outFile.write(filename+'\n')

# Hard coded result directories
resultDirs = {"random1000test"}

unsolved =[]

print("reading in data...")

for dir in resultDirs:
    for file in listdir("../../../results/SlidingTilePuzzle/sampleData/" + dir):
        if not file.endswith('.txt'):
            continue

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

        if h==999 or hs==999:
            unsolved.append(file)


print("unsolved count "+str(len(unsolved))) 

f = open("../../../results/SlidingTilePuzzle/sampleData/"+min(resultDirs)+"-statSummary.txt","w")

dump2file(unsolved,f)
