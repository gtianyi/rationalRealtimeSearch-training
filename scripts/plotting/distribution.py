#!/usr/bin/python
'''
This file read the data and generate the distribution file for DDNancy

Author: Tianyi Gu
Date: 08/27/2019
'''

import json
import os
import sys
from collections import OrderedDict, defaultdict

import dumpAndPlot


def printUsage():
    print "usage: python distribution.py <tile type> <training alg> <dump file>"
    print "tile type: uniform heavy inverse"
    print "training alg: wastar lsslrtastar"
    print "dump file: d(distribution) postd(poset search distribution)"


def getHgroup(h, roundH=False):
    if roundH:
        return round(h, 1)
    return h


def main():

    # Hard coded result directories
    tileType = sys.argv[1]

    trainigType = sys.argv[2]

    fileType = sys.argv[3]

    if fileType == "postd":
        with open("../../../results/SlidingTilePuzzle/sampleData/" + tileType +
                  "-samples-postSearch.json") as json_file:
            data = json.load(json_file)
            od = OrderedDict(
                sorted(data.items(), key=lambda item: float(item[0])))
            dumpAndPlot.createDistAndDump(od, tileType, fileType)
        return

    h_collection = defaultdict(list)

    print("reading in data for " + tileType)

    # ready the frequency counter file, this file is generated when
    # we collect 200 unique states for each h bucket
    frequencyCounter = defaultdict(int)
    print("reading in frequency file...")
    with open(
            "../../../results/SlidingTilePuzzle/sampleProblem/" + tileType +
            "/" + trainigType + "/0FrequencyCounter.txt",
            "r") as frequencycounterFile:
        #parse the frequency counter file
        for line in frequencycounterFile:
            line = line.split(" ")
            frequencyCounter[line[0]] = int(line[1])

    #parse the instance solution file
    print("reading in solution files...")
    allFiles = os.listdir("../../../results/SlidingTilePuzzle/sampleData/" +
                          tileType + "/" + trainigType)
    totalFiles = len(allFiles)
    for i, oneFile in enumerate(allFiles):
        with open(
                "../../../results/SlidingTilePuzzle/sampleData/" + tileType +
                "/" + trainigType + "/" + oneFile, "r") as f:

            h = 999999
            hs = 999999

            processed = i * 100.0 / totalFiles
            print "read file processed", "%.2f" % processed, "%"

            for line in f:
                line = line.replace('"', '')

                if "initial heuristic" in line:
                    for s in line.split():
                        if any(c.isdigit() for c in s):
                            h = eval(s)

                if "solution length" in line:
                    for s in line.split():
                        if any(c.isdigit() for c in s):
                            hs = eval(s)

            if h != 999999 and hs != 999999:
                h_collection[getHgroup(h, tileType == "inverse")].append({
                    "hstar":
                    hs,
                    "counter":
                    frequencyCounter[oneFile.split(".")[0]],
                    "instance":
                    oneFile.split(".")[0] + ".st",
                    "deltaH":
                    0
                })

    print("fix missing data...")
    nomissingHHSCollection = dumpAndPlot.fixMissing(h_collection,
                                                    tileType == "inverse")
    od = OrderedDict(sorted(nomissingHHSCollection.items()))

    if fileType == "d":
        dumpAndPlot.createDistAndDump(od, tileType, fileType)
        dumpAndPlot.dumphSamples(od, tileType)

    # dumpAndPlot.dumphhat2file(h_collection, tileType)

    # dumpAndPlot.plotHist(h_collection, tileType)


if __name__ == "__main__":
    if len(sys.argv) < 4:
        printUsage()
    else:
        main()
