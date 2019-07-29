from collections import OrderedDict
from collections import defaultdict
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np


def make_histrogram(h_value, hslistpair, file_dir):
    '''Make histrogram given h value and two hstart

    keyword arguments:
    h_value -- h value
    hslistpair -- hs values
    file_dir -- output dir
    '''
    sns.set(rc={
        'figure.figsize': (11, 8),
        'font.size': 26,
        'text.color': 'black'
    })

    iqr1 = np.subtract(*np.percentile(np.array(hslistpair[0]), [75, 25]))
    sample_number = len(hslistpair[0])
    bin_width = 2 * iqr1 / (sample_number**(1 / 3.0))
    bin_num = None if bin_width == 0.0 else int(
        (max(hslistpair[0]) - min(hslistpair[0])) / bin_width)
    print("bin_width:", bin_width)
    print("bin_num:", bin_num)

    print("iqr: ", iqr1)
    print("n: ", sample_number)

    ax_ps = sns.distplot(np.array(hslistpair[1]),
                         label="Post Search Belief",
                         kde=False,
                         norm_hist=True,
                         hist_kws={
                             "histtype": "step",
                             "linewidth": 1,
                             "alpha": 1,
                             "color": "b"
                         },
                         bins=bin_num)
    ax_ps.axvline(np.array(hslistpair[1]).mean(),
                  color='b',
                  linewidth=4,
                  ymin=0.45,
                  ymax=0.55)

    ax_or = sns.distplot(np.array(hslistpair[0]),
                         label="Original Belief",
                         kde=False,
                         norm_hist=True,
                         hist_kws={
                             "histtype": "step",
                             "linewidth": 1,
                             "alpha": 1,
                             "color": "r"
                         },
                         bins=bin_num)
    ax_or.axvline(np.array(hslistpair[0]).mean(),
                  color='r',
                  linewidth=4,
                  ymin=0.45,
                  ymax=0.55)

    #ax.tick_params(colors='black', labelsize=12, rotation=90)
    # ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
    # ax.set_xscale('log')
    # plt.ylabel("density", color='black', fontsize=18)
    plt.xlabel("h*", color='black', fontsize=18)
    plt.title("h=" + h_value)
    plt.legend()
    plt.savefig(file_dir + "h" + h_value.zfill(5) + ".eps",
                bbox_inches="tight",
                pad_inches=0)
    plt.close()
    plt.clf()
    plt.cla()
    return


def parse_data_file(fname):
    ''' parse data from a file

    keyworkd arguments:
    fname -- input file
    '''
    h_collection = defaultdict(list)

    input_file = open(fname, "r")

    for line in input_file:
        values = line.split()
        h_value = values[0]
        count = (int)(values[1])
        cur = 2
        while cur < len(values):
            cur_count = (int)(values[cur + 1])
            while cur_count > 0:
                h_collection[h_value].append((float)(values[cur]))
                cur_count = cur_count - 1
            cur = cur + 2
        assert len(h_collection[h_value]) == count

    return h_collection


def merge2map(map_1, map_2):
    '''merge two h->hs maps into one map h->[hs,hs]

    '''
    ret_map = defaultdict(list)
    for h_value, hs_value in map_1.iteritems():
        assert h_value in map_2
        ret_map[h_value].append(hs_value)
        ret_map[h_value].append(map_2[h_value])
        map_2.pop(h_value)
    assert not map_2
    return ret_map


# Hard coded data file
FILE_BELIEF="../../../results/SlidingTilePuzzle/sampleData/inverse-statSummary-20-0.1-200-backup.txt"
FILE_PS_BELIEF="../../../results/SlidingTilePuzzle/sampleData/inverse-statSummary-20-0.1-200-postSearch.txt"
# FILE_BELIEF = "../../../results/SlidingTilePuzzle/sampleData/uniform-statSummary.txt"
# FILE_PS_BELIEF = "../../../results/SlidingTilePuzzle/sampleData/uniform-statSummary-postSearch.txt"

print "reading in data..."
H_TO_HS = parse_data_file(FILE_BELIEF)
H_TO_PSHS = parse_data_file(FILE_PS_BELIEF)

H_TO_HSS_MAP = merge2map(H_TO_HS, H_TO_PSHS)

print "plotting..."

OD = OrderedDict(sorted(H_TO_HSS_MAP.items()))
PLOT_DIR ="../../../plots/hist/inverse/20-0.1-200/"
# PLOT_DIR = "../../../plots/hist/uniform/postSearch/"

for h, pair in OD.items():
    if len(pair[0]) > 1:
        make_histrogram(h, pair, PLOT_DIR)
