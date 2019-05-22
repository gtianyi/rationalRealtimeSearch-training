import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import json
import seaborn as sns
import numpy as np
import re
from os import listdir
from collections import defaultdict
from collections import OrderedDict

def makeHistrogram(h, hslistpair, fileDir):
    sns.set(rc={'figure.figsize': (11, 8), 'font.size': 26, 'text.color': 'black'})

    ax_ps = sns.distplot(np.array(hslistpair[1]), label="Post Search Belief", kde=False, norm_hist=True, hist_kws={"histtype": "step", "linewidth": 1,"alpha": 1, "color": "b"})
    ax_ps.axvline(np.array(hslistpair[1]).mean(), color='b', linewidth=4, ymin = 0.45, ymax=0.55)

    ax_or = sns.distplot(np.array(hslistpair[0]), label="Original Belief", kde=False, norm_hist=True, hist_kws={"histtype": "step", "linewidth": 1,"alpha": 1, "color": "r"})
    ax_or.axvline(np.array(hslistpair[0]).mean(), color='r', linewidth=4, ymin = 0.45, ymax=0.55)

    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    # plt.ylabel("density", color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.title("h="+ h )
    plt.legend()
    plt.savefig(fileDir+"h"+h.zfill(5)+".eps", bbox_inches="tight", pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return

def parseDataFile(fname):
    h_collection = defaultdict(list)

    f = open(fname, "r")

    for line in f:
        values = line.split()
        h = values[0]
        count = (int)(values[1])
        cur = 2
        while cur < len(values):
           curCount = (int)(values[cur+1]) 
           while curCount > 0:
               h_collection[h].append((float)(values[cur]))
               curCount=curCount-1
           cur = cur + 2 
        assert len(h_collection[h]) == count

    return h_collection

def merge2map(m1, m2):
    m = defaultdict(list)
    for h, l in m1.iteritems():
        assert(h in m2)
        m[h].append(l)
        m[h].append(m2[h])
        m2.pop(h)
    assert (len(m2)==0)
    return m


# Hard coded data file
# beliefFile="../../../results/SlidingTilePuzzle/sampleData/inverse-statSummary-20-0.1-200-backup.txt"
# postsearchbeliefFile="../../../results/SlidingTilePuzzle/sampleData/inverse-statSummary-20-0.1-200-postSearch.txt"
beliefFile="../../../results/SlidingTilePuzzle/sampleData/uniform-statSummary.txt"
postsearchbeliefFile="../../../results/SlidingTilePuzzle/sampleData/uniform-statSummary-postSearch.txt"

print("reading in data...")
belief_origin = parseDataFile(beliefFile)
belief_ps = parseDataFile(postsearchbeliefFile)

belief_together=merge2map(belief_origin, belief_ps)

print("plotting...")

od = OrderedDict(sorted(belief_together.items()))
# plotDir ="../../../plots/hist/inverse/20-0.1-200/"
plotDir ="../../../plots/hist/uniform/postSearch/"

for h, pair in od.items():
    if len(pair[0])>1 :
        makeHistrogram(h,pair,plotDir)
