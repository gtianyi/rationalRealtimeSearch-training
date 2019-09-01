#!/usr/bin/python
'''
This file read the data and generate the distribution file for DDNancy

Author: Tianyi Gu
Date: 08/27/2019
'''

import os
from collections import defaultdict

import dumpAndPlot


def main():

    # Hard coded result directories
    tileType = "heavy"

    h_collection = defaultdict(list)

    print("reading in data for " + tileType)

    # ready the frequency counter file, this file is generated when
    # we collect 200 unique states for each h bucket
    frequencyCounter = defaultdict(int)
    print("reading in frequency file...")
    with open(
            "../../../results/SlidingTilePuzzle/sampleProblem/" + tileType +
            "/0FrequencyCounter.txt", "r") as frequencycounterFile:
        #parse the frequency counter file
        for line in frequencycounterFile:
            line = line.split(" ")
            frequencyCounter[line[0]] = int(line[1])

    #parse the instance solution file
    print("reading in solution files...")
    allFiles = os.listdir("../../../results/SlidingTilePuzzle/sampleData/" +
                          tileType)
    totalFiles = len(allFiles)
    for i, oneFile in enumerate(allFiles):
        with open(
                "../../../results/SlidingTilePuzzle/sampleData/" + tileType +
                "/" + oneFile, "r") as f:

            h = 999999
            hs = 999999

            processed = i * 100.0 / totalFiles
            print "file processed", "%.2f" % processed, "%"

            for line in f:
                line = line.replace('"', '')

                if "initial heuristic" in line:
                    for s in line.split():
                        if s.isdigit():
                            h = int(s)

                if "solution length" in line:
                    for s in line.split():
                        if s.isdigit():
                            hs = int(s)

            if h != 999999 and hs != 999999:
                h_collection[h].append({
                    "hstar":
                    hs,
                    "counter":
                    frequencyCounter[oneFile.split(".")[0]],
                    "instance":
                    oneFile.split(".")[0] + ".st",
                    "deltaH":
                    0
                })

    # dumpAndPlot.dumphhstar(h_collection, tileType)

    # dumpAndPlot.dumphhat2file(h_collection, tileType)

    dumpAndPlot.dumphSamples(h_collection, tileType)

    # dumpAndPlot.plotHist(h_collection, tileType)


if __name__ == "__main__":
    main()
