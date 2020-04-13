#!/usr/bin/python
'''
This file read the data and generate the distribution file for DDNancy

Author: Tianyi Gu
Date: 08/27/2019
'''

import argparse
# import json
import os
# import re
# import sys
from collections import OrderedDict, defaultdict

import dumpAndPlot

# def printUsage():
# print ("usage: python distribution.py <tile type> <training alg> <dump file>")
# print "tile type: uniform heavy inverse"
# print "training alg: wastar lsslrtastar"
# print "dump file: d(distribution) postd(poset search distribution)"


def parseArugments():

    parser = argparse.ArgumentParser(description='distribution generator')
    parser.add_argument('-d',
                        action='store',
                        dest='domain',
                        help='domain: tile, pancake(default)',
                        default='pancake')
    parser.add_argument(
        '-s',
        action='store',
        dest='subdomain',
        help=
        'subdomain: tile: uniform, heavy, inverse, pancake: regular(default), heavy',
        default='regular')
    parser.add_argument('-u',
                        action='store',
                        dest='suboptimal_alg',
                        help='suboptimal solver:lsslrtastar, wastar(default)',
                        default='wastar')
    parser.add_argument('-p',
                        action='store',
                        dest='suboptimal_para',
                        help='suboptimal solver parameter (default: 2)',
                        default='2')
    parser.add_argument('-z',
                        action='store',
                        dest='size',
                        help='domain size (default: 16)',
                        default='16')
    parser.add_argument(
        '-t',
        action='store',
        dest='fileType',
        help='distribution data type, d(default) for distrubtuion, \
        postd for post search distribtuion ',
        default='d')

    return parser.parse_args()


def generatePostSearchData():
    print("TODO: fix generatePostSearchData for pancake")
    # with open("../../../results/SlidingTilePuzzle/sampleData/" + tileType +
    # "-" + trainingType +
    # "-samples-postSearch.json") as json_file:
    # data = json.load(json_file)
    # od = OrderedDict(
    # sorted(data.items(), key=lambda item: float(item[0])))
    # dumpAndPlot.createDistAndDump(od, tileType, fileType, trainingType)
    return


def getHgroup(h, roundH=False):
    if roundH:
        return round(h, 1)
    return h


def read_tiles():
    print("TODO: fix read_tiles for new argment list")
    # h_collection = defaultdict(list)
    # # ready the frequency counter file, this file is generated when
    # # we collect 200 unique states for each h bucket
    # frequencyCounter = defaultdict(int)

    # print("reading in frequency file...")
    # with open(
    # "../../../results/SlidingTilePuzzle/sampleProblem/" + tileType +
    # "/" + trainingType + "/0FrequencyCounter.txt",
    # "r") as frequencycounterFile:
    # #parse the frequency counter file
    # for line in frequencycounterFile:
    # line = line.split(" ")
    # frequencyCounter[line[0]] = int(line[1])

    # #parse the instance solution file
    # print("reading in solution files...")
    # allFiles = os.listdir("../../../results/SlidingTilePuzzle/sampleData/" +
    # tileType + "/" + trainingType)
    # totalFiles = len(allFiles)
    # for i, oneFile in enumerate(allFiles):
    # with open(
    # "../../../results/SlidingTilePuzzle/sampleData/" + tileType +
    # "/" + trainingType + "/" + oneFile, "r") as f:

    # h = 999999
    # hs = 999999

    # processed = i * 100.0 / totalFiles
    # print "read file ", oneFile, " processed ", "%.2f" % processed, "%"

    # for line in f:
    # line = line.replace('"', '')

    # if "initial heuristic" in line:
    # for s in line.split():
    # if any(c.isdigit() for c in s):
    # h = eval(s)

    # if "solution length" in line:
    # #this only works for uniform and heavy
    # # to prevent the bug of read "solution lenght 213end"
    # hs = eval(re.findall(r"\d+", line)[0])

    # # for s in line.split():
    # # if any(c.isdigit() for c in s):
    # # hs = eval(s)

    # if h != 999999 and hs != 999999:
    # h_collection[getHgroup(h, tileType == "inverse")].append({
    # "hstar":
    # hs,
    # "counter":
    # frequencyCounter[oneFile.split(".")[0]],
    # "instance":
    # oneFile.split(".")[0] + ".st",
    # "deltaH":
    # 0
    # })

    # print("fix missing data...")
    # nomissingHHSCollection = dumpAndPlot.fixMissing(h_collection,
    # tileType == "inverse")
    # od = OrderedDict(sorted(nomissingHHSCollection.items()))
    return OrderedDict()


def read_pancake(args):
    h_collection = defaultdict(list)
    # ready the frequency counter file, this file is generated when
    # we collect 200 unique states for each h bucket
    frequencyCounter = defaultdict(int)

    print("reading in frequency file...")
    with open(
            "../../../results/" + args.domain + "/sampleProblem/" +
            args.subdomain + "/" + args.suboptimal_alg + "/Para" +
            args.suboptimal_para + "/" + args.size + "/0FrequencyCounter.txt",
            "r") as frequencycounterFile:
        #parse the frequency counter file
        for line in frequencycounterFile:
            line = line.split(" ")
            frequencyCounter[line[0]] = int(line[1])

    #parse the instance solution file
    sol_file_dir = "../../../results/" + args.domain + "/sampleData/" +\
        args.subdomain + "/" + args.suboptimal_alg + "/" + args.suboptimal_para + "/" + args.size

    allFiles = os.listdir(sol_file_dir)

    totalFiles = len(allFiles)

    print("reading in solution files, total file #: " + str(totalFiles))

    for i, oneFile in enumerate(allFiles):
        # if i > 2:
            # break
        with open(sol_file_dir + "/" + oneFile, "r") as f:

            h = 999999
            hs = 999999

            processed = i * 100.0 / totalFiles
            print "read file ", oneFile, " processed ", "%.2f" % processed, "%"

            info = f.readline().split()
            h = info[3]
            hs = info[2]

            print info

            if info[1] == "1" and h != 999999 and hs != 999999:
                h_collection[getHgroup(h)].append({
                    "hstar":
                    hs,
                    "counter":
                    frequencyCounter[oneFile.split(".")[0]],
                    "instance":
                    oneFile.split(".")[0] + ".st",
                    "deltaH":
                    0
                })

    # print (h_collection)
    # print("fix missing data...")
    # nomissingHHSCollection = dumpAndPlot.fixMissing(h_collection)
    # od = OrderedDict(sorted(nomissingHHSCollection.items()))
    return OrderedDict(sorted(h_collection.items()))


def main():

    args = parseArugments()
    print(args)

    if args.fileType == "postd":
        generatePostSearchData()
        return

    print("reading in data")

    od = OrderedDict()

    if args.domain == "tile":
        od = read_tiles()
    elif args.domain == "pancake":
        od = read_pancake(args)

    if args.fileType == "d":
        dumpAndPlot.createDistAndDump(od, args)
        dumpAndPlot.dumphSamples(od, args)

    # dumpAndPlot.dumphhat2file(h_collection, tileType)

    # dumpAndPlot.plotHist(h_collection, tileType)


if __name__ == "__main__":
    main()
