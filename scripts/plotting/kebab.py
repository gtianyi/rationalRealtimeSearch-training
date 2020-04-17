#!/usr/bin/env python

from math import sqrt,log, floor
import matplotlib.pyplot as plt
import sys
import os

from collections import defaultdict

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
    labels = []
    for line in f:
        if line == "\n":
            break
        vals = line.split(" ")
        h = vals[0]
        labels.append(h)
        num_points = int(vals[1])
        hstars = defaultdict(float)
        i = 2
        while i < len(vals):
            if len(vals[i].strip()) == 0:
                i += 1
                continue
            hstar = float(vals[i])
            cnt = int(vals[i+1])
            prob = cnt / num_points
            hstars[hstar] += prob
            i += 2
        res[h] = list(hstars.items())
    return (res, labels)

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
    if m == 0:
        m = 0.000001
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
    for (i, (h,distr)) in enumerate(sorted(hists.items())):
        ys = ypos(distr)
        height = ypos(distr)[1] - ypos(distr)[0]
        w = widths(distr)
        l = left_starts(i, w, centered)
        axs.barh(ys, width=w, height=height, left=l, color="#d43f3a", edgecolor="#000000", linewidth=0.5)
        if show_means:
            e = mean(distr)
            mean_w = 0.12
            x1 = i - 0.03
            if centered:
                x1 -= mean_w * 0.5
            x2 = x1 + 0.12
            axs.plot([x1,x2],[e,e], color="#200000", linewidth=2)


def main(file_name, out_file=None):
    global axs
    grid=True
    centered=True
    rebucket = True
    with open(file_name, "r") as f:
        if out_file is None:
            out_dir = file_name[:-4] + "_plots"
            out_file= os.path.join(out_dir, "hist.png")
            os.path.exists(out_dir) or os.makedirs(out_dir)
        data, labels = read_data(f)

    if rebucket:
        for k in data.keys():
            n = len(data[k])
            b = floor(1 + 3.322 * log(n))
            if b <= 2:
                b = 3
            size = max([x[0] for x in data[k]]) / b
            while size < 1 and b >= 3:
                b -= 1
                size = max([x[0] for x in data[k]]) / b
            nd = []
            for i in range(b):
                low = i*size
                high = (i+1)*size
                accprob = sum([x[1] for x in data[k] if x[0] >= low and x[0] <= high])
                nd.append((i*size, accprob))
            data[k] = nd

    figscalew = 17
    figscaleh = 2
    figsize = (figscalew, 14 / figscaleh)
    fig, axs = plt.subplots(figsize=figsize)
    axs.tick_params(axis="both", which="major", labelsize=fs)
    axs.tick_params(axis="both", which="minor", labelsize=fs)
    if grid:
        axs.grid(grid, alpha=0.2, color="#a0a0a0", linewidth=1)
    xlabel = " "
    ylabel = file_name.replace('data/', '').replace('.dat', '').replace('_', ' ').title()

    make_bars(data, centered, show_means=False)

    axs.set_xticks(range(0, len(labels)))
    axs.set_xticklabels(labels)
    axs.set_xlabel(xlabel)
    axs.set_ylabel(ylabel)

    #plt.show()
    fig.savefig(out_file, bbox_inches="tight", dpi=dpi)
    fig.clf()


if __name__=='__main__':
    for file_name in sys.argv[1:]:
        main(file_name)
