'''
sctipts for convert tile from postion to face

Author: Tianyi Gu
Date: 03/17/2020
'''

#!/usr/bin/env python

__author__ = 'tianyigu'

import sys


def printUsage():
    print("usage: python tile-converter.py input output")


def main():
    if len(sys.argv) != 3:
        printUsage()
        return

    input_file = open(sys.argv[1], 'r')
    in_lines = input_file.readlines()
    out_lines = in_lines[:]

    in_tiles = [int(i) for i in in_lines[2:18]]
    out_tile = [0 for _ in range(16)]
    for i, pos in enumerate(in_tiles):
        out_tile[pos] = i

    for i, t in enumerate(out_tile):
        out_lines[i + 2] = str(t)+'\n'

    with open(sys.argv[2], 'w') as filehandle:
        filehandle.writelines("%s" % place for place in out_lines)

if __name__ == '__main__':
    main()
