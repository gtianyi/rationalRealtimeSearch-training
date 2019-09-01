#!/usr/bin/python
'''
This file includes some helper functions for collecting the training results

Author: Tianyi Gu
Create Date: 05/22/2019
'''

import random
from collections import OrderedDict

from datetime import datetime
import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

__author__ = 'Tianyi Gu'

#---------- Dump out h-hstar ---------------------------


def dump2file(h, hs, outFile):
    hsSet = sorted(set([x["hstar"] for x in hs]))
    # not how many unique states
    # but total frequency counter
    outFile.write(str(h) + ' ' + str(sum([x["counter"] for x in hs])) + ' ')
    for hsvalue in hsSet:
        hsCounters = [x["counter"] for x in hs if x["hstar"] == hsvalue]
        valueCount = sum(hsCounters)
        outFile.write(str(hsvalue) + ' ' + str(valueCount) + ' ')
    outFile.write("\n")


def dumphhstar(hhsCollection, dirName, roundHS=False):
    print("fix missing and dumping out h-hstar collection...")
    print("h count " + str(len(hhsCollection)))

    f = open(
        "../../../results/SlidingTilePuzzle/sampleData/" + dirName +
        "-statSummary.txt", "w")

    nomissingHHSCollection = fixMissing(hhsCollection, roundHS)
    od = OrderedDict(sorted(nomissingHHSCollection.items()))

    for h, hslist in od.items():
        dump2file(h, hslist, f)


def fixMissing(hhsCollection, roundHS):
    od = OrderedDict(sorted(hhsCollection.items()))

    hStep = 1
    if roundHS:
        hStep = 0.1
        for h, hslist in od.items():
            #save to 0.1 to make histogram
            for x in hslist:
                x.update("hstar", round(x["hstar"], 2))

    nomissingHHSCollection = {}

    # fix missing data and dump
    prevH = -hStep

    for h, hslist in od.items():

        hCur = prevH + hStep

        lowH = None if prevH < 0 else prevH
        highH = None if od.keys().index(h) == len(
            od.keys()) - 1 else od.keys()[od.keys().index(h) + 1]

        while (hCur <= h):

            highHCur = highH if hCur == h else h

            nomissingHS = getNoMissingHSOfH(hCur, lowH, highHCur, od)

            print("hcur ", hCur, "lowH ", lowH, "hghH ", highHCur,
                  "data size ", len(nomissingHS))

            nomissingHHSCollection[hCur] = nomissingHS

            hCur = hCur + hStep

        prevH = h

    return nomissingHHSCollection


def getNoMissingHSOfH(h, lowH, highH, od):
    nomissingHS = od[h][:] if h in od else []

    #go down and pull up
    if len(nomissingHS) < 200 and lowH != None:

        smallHHSCollection = OrderedDict(
            sorted(od.items()[:od.keys().index(lowH)], reverse=True))
        grabDataFromOrderedCollection(smallHHSCollection, nomissingHS, h)

    #go up and pull down
    if len(nomissingHS) < 200 and highH != None:

        largeHHSCollection = OrderedDict(od.items()[od.keys().index(highH):])
        grabDataFromOrderedCollection(largeHHSCollection, nomissingHS, h)

    return nomissingHS


def grabDataFromOrderedCollection(orderdCollection, nomissingHS, nomissingh):
    for h in orderdCollection:
        random.seed(19)
        random.shuffle(orderdCollection[h])
        hs_list = orderdCollection[h]

        for instance in hs_list:
            if len(nomissingHS) >= 200:
                return

            deltah = nomissingh - h
            if instance["hstar"] + deltah < nomissingh:
                print "h to fix ", nomissingh, "pull down from ", h, "instance", instance
            shifted = shiftInstance(instance, deltah)
            nomissingHS.append(shifted)


def shiftInstance(instance, v):
    newinstance = instance.copy()
    newinstance.update({"hstar": instance["hstar"] + v})
    newinstance.update({"deltaH": v})
    return newinstance


#---------- Dump out h-hat ---------------------------


def dumphhat(h, hs, outFile):
    outFile.write(str(h) + ' ' + str(sum(hs) / len(hs)))
    outFile.write("\n")


def dumphhat2file(hhatCollection, dirName):
    print("dumping out h-hat collection...")
    print("h count " + str(len(hhatCollection)))

    f_hhat = open(
        "../../../results/SlidingTilePuzzle/sampleData/" + dirName +
        "-hhat.txt", "w")
    od = OrderedDict(sorted(hhatCollection.items()))

    for h, hslist in od.items():
        dumphhat(h, hslist, f_hhat)


#---------- Dump out h and samples ---------------------------


def dump2file_sample_states(h, samples, outFile):
    outFile.write(str(h) + ' ' + str(len(samples)) + ' ')
    for sample in samples:
        outFile.write(sample["instance"] + ' ' + str(sample["deltaH"]) + " ")
    outFile.write("\n")


def dumphSamples(sampleCollection, dirName, roundHS=False):
    print("dumping out h-samples collection...")
    print("h count " + str(len(sampleCollection)))
    nomissingHHSCollection = fixMissing(sampleCollection, roundHS)
    od = OrderedDict(sorted(nomissingHHSCollection.items()))

    f_samples = open(
        "../../../results/SlidingTilePuzzle/sampleData/" + dirName +
        "-samples.txt", "w")

    for h, samples in od.items():
        dump2file_sample_states(h, samples, f_samples)


#---------- plotting Hist---------------------------


def makeHistrogram(h, hs, fileDir, roundHS=False):
    sns.set(rc={
        'figure.figsize': (11, 8),
        'font.size': 26,
        'text.color': 'black'
    })
    # ax = sns.distplot(np.array(hs), kde=False)
    sns.distplot(np.array([x["hstar"] for x in hs]), kde=False)
    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    ylable = ''
    if roundHS:
        ylable = '%.2f' % h
    else:
        ylable = str(h)

    plt.ylabel("h=" + ylable, color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.savefig(fileDir + "h" + str(h).zfill(5) + ".eps",
                bbox_inches="tight",
                pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return


def plotHist(hhsCollection, tileType, roundHS=False):
    print("plot hist...")
    print("h count " + str(len(hhsCollection)))

    dtString = datetime.now().strftime("%d%m%Y-%H%M")
    plotDir = "../../../plots/hist/" + tileType + "/" + dtString + "/"

    if not os.path.exists(plotDir):
        os.makedirs(plotDir)

    nomissingHHSCollection = fixMissing(hhsCollection, roundHS)
    od = OrderedDict(sorted(nomissingHHSCollection.items()))

    for h, hslist in od.items():
        if hslist:
            makeHistrogram(h, hslist, plotDir, roundHS)


#---------- plotting Scatter---------------------------


def makeScatterPlot(h, hs, fileDir):
    df = pd.DataFrame({"h": h, "h*": hs})
    sns.set(rc={
        'figure.figsize': (11, 8),
        'font.size': 26,
        'text.color': 'black'
    })
    # ax = sns.jointplot(x="h",y="h*",data=df, kind = "hex")
    sns.scatterplot(x="h", y="h*", data=df, alpha=0.01)
    plt.ylabel("h*", color='black', fontsize=18)
    plt.xlabel("h", color='black', fontsize=18)
    plt.savefig(fileDir + "scatter-inverse.pdf",
                bbox_inches="tight",
                pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return


def plotScatter(all_h, all_hs, dirName):
    plotDir = "../../../plots/hist/" + dirName + "/"
    makeScatterPlot(all_h, all_hs, plotDir)


#---------- Dump point to csv ---------------------------


def dumpPoints2csv(h, hs, fileDir):
    df = pd.DataFrame({"h": h, "h*": hs})
    df.to_csv(fileDir + "h-h*.csv", index=None, header=True)
    return


def dump2csv(all_h, all_hs, dirName):
    csvDir = "../../../results/SlidingTilePuzzle/sampleData/" + dirName + "/"
    dumpPoints2csv(all_h, all_hs, csvDir)
