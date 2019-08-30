#!/usr/bin/python
'''
This file read the two distribution files and smooth the data by
erasing data if its hhat decrease

Author: Tianyi Gu
Date: 08/29/2019
update: 08/30/2019, discard this file, we decide not to do smooth any more
'''

import sys


def printUsage():
    print "usage: python distribution-smoothing.py <tile type>"
    print "tile type: uniform heavy inverse"


def smooth_data_file(fname):
    ''' parse data from a file and erase lines that their hhat decrease
    format in each line:
    <h> <# of data points> <data_1> <count_1> <data_2> <count_2> ...

    keyworkd arguments:
    fname -- input file
    '''
    erased_h = []

    input_file = open(fname, "r")
    out_file = open(fname[:-4] + "-smoothed.txt", "w")

    prevHHat = 0
    for line in input_file:
        values = line.split()
        h_value = values[0]
        hsvalues = []
        count = (int)(values[1])
        cur = 2
        while cur < len(values):
            cur_count = (int)(values[cur + 1])
            while cur_count > 0:
                hsvalues.append((float)(values[cur]))
                cur_count = cur_count - 1
            cur = cur + 2
        assert len(hsvalues) == count
        if sum(hsvalues) / len(hsvalues) < prevHHat:
            erased_h.append(h_value)
        else:
            out_file.write(line)
            prevHHat = sum(hsvalues) / len(hsvalues)


    return erased_h


def smooth_data_file_ps(fname, hList):
    ''' erase all lines with hvalue in hList

    keyworkd arguments:
    fname -- input file
    fname -- h list
    '''
    input_file = open(fname, "r")
    out_file = open(fname[:-4] + "-smoothed.txt", "w")

    for line in input_file:
        values = line.split()
        h_value = values[0]
        if h_value not in hList:
            out_file.write(line)

def main():

    if len(sys.argv) != 2:
        printUsage()
        return

    FILE_BELIEF = "../../../results/SlidingTilePuzzle/sampleData/" + sys.argv[
        1] + "-statSummary-nomissing.txt"
    FILE_PS_BELIEF = "../../../results/SlidingTilePuzzle/sampleData/" + sys.argv[
        1] + "-statSummary-postSearch-nomissing.txt"

    print "dumpping..."

    erased_h = smooth_data_file(FILE_BELIEF)
    smooth_data_file_ps(FILE_PS_BELIEF, erased_h)


if __name__ == "__main__":
    main()
