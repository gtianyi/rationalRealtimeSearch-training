#!/usr/bin/env python

from math import sqrt,log
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys
import os

if len(sys.argv) != 3:
    print("Gimme 2 arguments")
    sys.exit(1)

nicenames = {
    "blocks.txt": "Blocksworld",
    "transport_unit.txt": "Transport (unit)",
    "blocks_l.txt": "Blocksworld (LSS-LRTA*)",
    "blocks_r2_conv.txt": "Blocksworld (half as many instances)",
}

# dots per inch
dpi=200
# font size
fscale=1.65
fs=fscale*max(sqrt(dpi)*0.5, 9)
color1, color2 = "#d43f3a", "#3034d4"

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
def make_bars(hists, centered, alpha, color=color1, show_means=True):
    for h,distr in sorted(hists.items()):
        ys = ypos(distr)
        w = widths(distr)
        l = left_starts(h, w, centered)
        axs.barh(ys, width=w, height=1.0, left=l, alpha=alpha, color=color, edgecolor="#000000", linewidth=0.5)
        if show_means:
            e = mean(distr)
            mean_w = 0.12
            x1 = h - 0.03
            if centered:
                x1 -= mean_w * 0.5
            x2 = x1 + 0.12
            axs.plot([x1,x2],[e,e], color="#200000", linewidth=2)


grid=True
centered=True
file_name1 = sys.argv[1]
file_name2 = sys.argv[2]
out_dir = file_name1[:-4] + "_plots"
os.path.exists(out_dir) or os.makedirs(out_dir)
out_file= os.path.join(out_dir, "vs" + file_name2[:-4] + "kebab.png")

with open(file_name1, "r") as f:
    data1, max_h1 = read_data(f)
with open(file_name2, "r") as f:
    data2, max_h2 = read_data(f)

max_h = max(max_h1, max_h2)
figscalew = 17
figscaleh = 2
figsize = (14 / figscaleh, figscalew)
figsize = (figsize[1], figsize[0])
fig, axs = plt.subplots(figsize=figsize)
axs.tick_params(axis="both", which="major", labelsize=fs)
axs.tick_params(axis="both", which="minor", labelsize=fs)
if grid:
    axs.grid(grid, alpha=0.2, color="#a0a0a0", linewidth=1)
axs.legend([patches.Patch(color=color1), patches.Patch(color=color2)],
          [nicenames[file_name1], nicenames[file_name2]],
          loc="upper left")
xlabel = "h"
ylabel = "h* distribution"

make_bars(data1, centered, 0.75, color=color1)
make_bars(data2, centered, 0.8,  color=color2)

labels = list(range(0,max_h+1))
axs.set_xticks(labels)
axs.set_xticklabels(labels)
axs.set_xlabel(xlabel, fontsize=fs)
axs.set_ylabel(ylabel, fontsize=fs)

#plt.show()
fig.savefig(out_file, bbox_inches="tight", dpi=dpi)
fig.clf()
