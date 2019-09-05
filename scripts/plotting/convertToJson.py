#!/usr/bin/python
'''
This file read the text data and convert it to json format,
so we don't have to re-generate all data

Author: Tianyi Gu
Date: 09/04/2019
'''

import json
import sys

def printUsage():
    print "usage: python distribution.py <tile type> <dump file>"
    print "tile type: uniform heavy inverse"
    print "dump file: d(distribution) postd(poset search distribution)"


def main():

    # Hard coded result directories
    oldF = sys.argv[1]

    dist = {}
    with open("../../../results/SlidingTilePuzzle/sampleData/" +
              oldF) as text_file:
        for line in text_file:
            line = line.split()
            h = line[0]
            number_of_bins = eval(line[1])
            bins = []

            i = 2
            while i < len(line):
                bins.append({
                    "h*":
                    int(eval(line[i])),
                    "prob":
                    "%.6f" % (eval(line[i + 1])*1.0 / number_of_bins)
                })
                i += 2
            dist[h] = {"number of bins": number_of_bins, "bins": bins}

    with open(
            "../../../results/SlidingTilePuzzle/sampleData/" + oldF + ".json",
            "w") as json_file:
        json.dump(dist, json_file)


if __name__ == "__main__":
    main()
