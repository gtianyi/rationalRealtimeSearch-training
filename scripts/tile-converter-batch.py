'''
sctipts for convert tile from postion to face

Author: Tianyi Gu
Date: 03/17/2020
update: 09/28/2020
'''

#!/usr/bin/python

__author__ = 'tianyigu'

import sys
import os


def printUsage():
    print("usage: python3 tile-converter.py")


def main():
    if len(sys.argv) != 1:
        printUsage()
        return

    for inFile in os.listdir("./"):
        if inFile[-2:] !='st':
            continue

        with open("./"+inFile) as input_file:
            in_lines = input_file.readlines()
            out_lines = in_lines[:]

            in_tiles = [int(i) for i in in_lines[2:18]]
            out_tile = [0 for _ in range(16)]
            for i, pos in enumerate(in_tiles):
                out_tile[pos] = i

            for i, t in enumerate(out_tile):
                out_lines[i + 2] = str(t)+'\n'

            with open("../slidingTileAdd_converted/"+inFile+".new", 'w') as outfile:
                outfile.writelines("%s" % place for place in out_lines)

if __name__ == '__main__':
    main()
