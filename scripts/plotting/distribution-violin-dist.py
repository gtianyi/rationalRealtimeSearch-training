#!/usr/bin/python
'''
This file read the one distribution file and generate the violin plot

Author: Tianyi Gu
Date: 03/06/2020
'''

import json
import os
# import sys
# from collections import OrderedDict, defaultdict
from datetime import datetime

import matplotlib.pyplot as plt
import pandas as pd
# import numpy as np
import seaborn as sns


def printUsage():
    print "usage: python distribution-plot-2dist.py <tile type>"
    print "tile type: uniform heavy inverse"


def make_violin_plot(dict_data, file_dir):
    '''Make violin plot given two distribution dicts

    keyword arguments:
    dict_data -- dictionary of trained and assumption data
    file_dir -- output dir
    '''
    sns.set(rc={
        'figure.figsize': (8, 8),
        'font.size': 26,
        'text.color': 'black'
    })

    sns.violinplot(x="h", y="hstar", hue="name", data=dict_data)

    # plt.xlabel("h", color='black', fontsize=18)
    plt.legend()
    plt.savefig(file_dir + "violin.eps", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return


def makeupSamples(dist, name):
    totoalSample = 1000

    h_list = []
    hstar_list = []

    for h in dist:
        print("create sample for h: ", h, dist[h]["number of bins"])
        # if int(h) < 56:
        for onebin in dist[h]["bins"]:
            for _ in range(int(totoalSample * float(onebin["prob"]))):
                h_list.append(int(float(h)))
                hstar_list.append(int(float(onebin["h*"])))

    df = pd.DataFrame({"h": h_list, "hstar": hstar_list, "name": name})

    return df


def main():

    # if len(sys.argv) != 2:
    # printUsage()
    # return

    domain = "racetrack"
    in_file_Path = "../../../results/" + domain + "/sampleData/"
    in_file_Name = "barto-bigger-wastar-2--1-statSummary-d.json"

    FILE_BELIEF_DATA = in_file_Path + in_file_Name

    print "reading in data..."
    df = pd.DataFrame()

    with open(FILE_BELIEF_DATA) as json_file:
        df = makeupSamples(json.load(json_file), "trained")

    print df

    print "plotting..."

    dtString = datetime.now().strftime("%d%m%Y-%H%M")

    PLOT_DIR = "../../../plots/violin/" + domain + "/"
    out_file = PLOT_DIR + in_file_Name + dtString

    if not os.path.exists(PLOT_DIR):
        os.makedirs(PLOT_DIR)

    make_violin_plot(df, out_file)


if __name__ == "__main__":
    main()
