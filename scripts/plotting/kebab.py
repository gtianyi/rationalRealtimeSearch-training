#!/usr/bin/env python

import json
import os
# import sys
from collections import defaultdict
from math import sqrt
#, floor, log

import matplotlib.pyplot as plt

# dots per inch
dpi = 200
# font size
fscale = 1.65
fs = fscale * max(sqrt(dpi) * 0.5, 9)

# def makeupSamples(dist, name):
# totoalSample = 1000

# h_list = []
# hstar_list = []

# for h in dist:
# print("create sample for h: ", h, dist[h]["number of bins"])
# # if int(h) < 56:
# for onebin in dist[h]["bins"]:
# for _ in range(int(totoalSample * float(onebin["prob"]))):
# h_list.append(int(float(h)))
# hstar_list.append(int(float(onebin["h*"])))

# df = pd.DataFrame({"h": h_list, "hstar": hstar_list, "name": name})

# return df


def read_data(infile):
    print "reading in data..."
    ret = {}
    h_list = []

    with open(infile) as json_file:
        dist = json.load(json_file)
        for h in dist:
            h_list.append(int(float(h)))
            print("read hist for h: ", h, dist[h]["number of bins"])

            hstar_list = defaultdict(float)
            for onebin in dist[h]["bins"]:
                hstar_list[int(float(onebin["h*"]))] = float(onebin["prob"])

            ret[int(float(h))] = list(hstar_list.items())

    return (ret, sorted(h_list))


# The barh() function needs to know the left position where each bar
# starts.  When drawing the bars centered, each bars left position is
# computed by subtracting half its width.
def left_starts(i, ws, centered):
    if centered:
        return [i - 0.5 * w for w in ws]
    return i


# the y position of each bar is just the h* value of each bin.
def ypos(l):
    return [x[0] for x in l]


# the width of the bar is determined by the probability of each
# histogram bin.  dividing by the maximum probability normalizes the
# widths of the bars.  without normalization they are a little hard to
# look at.
def widths(l):
    m = max([x[1] for x in l])
    if m == 0:
        m = 0.000001
    # return list(map(lambda x: 0.65 * x[1] / m, l))
    return [0.65 * x[1] / m for x in l]


def mean(distr):
    res = 0
    for s in distr:
        res += s[0] * s[1]
    return res


# the barh() function needs the following parameters to draw a
# horizontal bar: 1. y position, 2. width, 3. height, 4. left
# position.  Some of these parameters are here given as arrays to draw
# multiple bars at once.
def make_bars(hists, centered, axs, show_means=True):
    for (i, (_, distr)) in enumerate(sorted(hists.items())):
        ys = ypos(distr)
        # height = ypos(distr)[1] - ypos(distr)[0]
        height = 1
        w = widths(distr)
        l = left_starts(i, w, centered)
        axs.barh(ys,
                 width=w,
                 height=height,
                 left=l,
                 color="#d43f3a",
                 edgecolor="#000000",
                 linewidth=0.5)
        if show_means:
            e = mean(distr)
            mean_w = 0.12
            x1 = i - 0.03
            if centered:
                x1 -= mean_w * 0.5
            x2 = x1 + 0.12
            axs.plot([x1, x2], [e, e], color="#200000", linewidth=2)


def main():
    # global axs
    grid = True
    centered = True
    # rebucket = True

    domain = "SlidingTilePuzzle"
    in_file_Path = "../../../results/" + domain + "/sampleData/"
    # in_file_Name = "uniform-wastar-2-40-statSummary-d.json"
    in_file_Name = "uniform-wastar-statSummary-d.json"

    FILE_BELIEF_DATA = in_file_Path + in_file_Name

    out_dir = "../../../plots/kebab/"+domain+"/"
    out_file = os.path.join(out_dir, in_file_Name+".kebab.png")

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    data, labels = read_data(FILE_BELIEF_DATA)

    # if rebucket:
    # for k in data:
    # n = len(data[k])
    # b = int(floor(1 + 3.322 * log(n)))
    # if b <= 2:
    # b = 3
    # size = max([x[0] for x in data[k]]) / b
    # while size < 1 and b >= 3:
    # b -= 1
    # size = max([x[0] for x in data[k]]) / b
    # nd = []
    # for i in range(b):
    # low = i * size
    # high = (i + 1) * size
    # accprob = sum(
    # [x[1] for x in data[k] if x[0] >= low and x[0] <= high])
    # nd.append((i * size, accprob))
    # data[k] = nd

    figscalew = 17
    figscaleh = 2
    figsize = (figscalew, 14 / figscaleh)
    fig, axs = plt.subplots(figsize=figsize)
    axs.tick_params(axis="both", which="major", labelsize=fs)
    axs.tick_params(axis="both", which="minor", labelsize=fs)
    if grid:
        axs.grid(grid, alpha=0.2, color="#a0a0a0", linewidth=1)
    xlabel = "h"
    # ylabel = file_name.replace('data/', '').replace('.dat', '').replace('_', ' ').title()
    ylabel = "h*"

    make_bars(data, centered, axs)

    axs.set_xticks(range(0, len(labels)))
    axs.set_xticklabels(labels)
    axs.set_xlabel(xlabel)
    axs.set_ylabel(ylabel)

    #plt.show()
    fig.savefig(out_file, bbox_inches="tight", dpi=dpi)
    fig.clf()


if __name__ == '__main__':
    main()
