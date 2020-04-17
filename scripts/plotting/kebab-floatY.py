#!/usr/bin/env python

from math import sqrt,log
import matplotlib.pyplot as plt
import sys
import os

# dots per inch
dpi=200
# font size
fscale=1.65
fs=fscale*max(sqrt(dpi)*0.5, 9)

# This reads the data in a list of pairs [(h*, prob), ...] and also
# returns the maximum h value.
def read_data(f):
    res = {}
    res_list = []
    max_h = 0
    for line in f:
        if line == "\n":
            break
        vals = line.split(" ")
        h = int(vals[0])
        if h > max_h:
           max_h = h
        num_points = int(vals[1])
        hstars = []
        i = 2
        while i < len(vals):
            hstar = int(vals[i])
            cnt = int(vals[i+1])
            prob = cnt / num_points
            hstars.append((hstar, prob))
            i += 2
        res[h] = hstars
    return (res, max_h)

# The barh() function needs to know the left position where each bar
# starts.  When drawing the bars centered, each bars left position is
# computed by subtracting half its width.
def left_starts(i, ws, centered):
    if centered:
        return list(map(lambda w: i-0.5*w, ws))
    else:
        return i

# the y position of each bar is just the h* value of each bin.
def ypos(l):
    return list(map(lambda x: x[0], l))

# the width of the bar is determined by the probability of each
# histogram bin.  dividing by the maximum probability normalizes the
# widths of the bars.  without normalization they are a little hard to
# look at.
def widths(l):

    m =  max(map(lambda x: x[1], l))
    return list(map(lambda x: 0.65*x[1]/m, l))

def mean(distr):
    res = 0
    for s in distr:
        res += s[0]*s[1]
    return res

# the barh() function needs the following parameters to draw a
# horizontal bar: 1. y position, 2. width, 3. height, 4. left
# position.  Some of these parameters are here given as arrays to draw
# multiple bars at once.
def make_bars(hists, centered, show_means=True):
    for h,distr in sorted(hists.items()):
        ys = ypos(distr)
        w = widths(distr)
        l = left_starts(h, w, centered)
        axs.barh(ys, width=w, height=1.0, left=l, color="#d43f3a", edgecolor="#000000", linewidth=0.5)
        if show_means:
            e = mean(distr)
            mean_w = 0.12
            x1 = h - 0.03
            if centered:
                x1 -= mean_w * 0.5
            x2 = x1 + 0.12
            axs.plot([x1,x2],[e,e], color="#200000", linewidth=2)


for file_name in sys.argv[1:]:
    grid=True
    centered=True
    with open(file_name, "r") as f:
        out_dir = file_name[:-4] + "_plots"
        out_file= os.path.join(out_dir, "hist.png")
        os.path.exists(out_dir) or os.makedirs(out_dir)
        data, max_h = read_data(f)

    figscalew = 17
    figscaleh = 2
    figsize = (figscalew, 14 / figscaleh)
    fig, axs = plt.subplots(figsize=figsize)
    axs.tick_params(axis="both", which="major", labelsize=fs)
    axs.tick_params(axis="both", which="minor", labelsize=fs)
    if grid:
        axs.grid(grid, alpha=0.2, color="#a0a0a0", linewidth=1)
    xlabel = "h"
    ylabel = "h* distribution"

    make_bars(data, centered)

    labels = list(range(0,max_h+1))
    axs.set_xticks(labels)
    axs.set_xticklabels(labels)
    axs.set_xlabel(xlabel)
    axs.set_ylabel(ylabel)

    #plt.show()
    fig.savefig(out_file, bbox_inches="tight", dpi=dpi)
    fig.clf()
