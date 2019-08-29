#!/usr/bin/python
'''
This file read the data and generate the distribution file for DDNancy

Author: Tianyi Gu
Date: 08/27/2019
'''

from collections import defaultdict
import os
import dumpAndPlot


def main():

    # Hard coded result directories
    tileType = "heavy"

    h_collection = defaultdict(list)
    h_collection_sampleStates = defaultdict(list)

    print("reading in data...")

    for oneFile in os.listdir(
            "../../../results/SlidingTilePuzzle/sampleData/" + tileType):
        f = open(
            "../../../results/SlidingTilePuzzle/sampleData/" + tileType + "/" +
            oneFile, "r")

        h = 999999
        hs = 999999

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
            h_collection[h].append(hs)
            h_collection_sampleStates[h].append(oneFile.split(".")[0] + ".st")

    # dumpAndPlot.dumphhstar(h_collection, tileType)

    dumpAndPlot.dumphSamples(h_collection_sampleStates, tileType)

    # dumpAndPlot.plotHist(h_collection, tileType)


if __name__ == "__main__":
    main()
