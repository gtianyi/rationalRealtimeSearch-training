// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include <sys/stat.h>
#include <cstring>
#include <memory>
#include "astar.hpp"
#include "heavyTiles-pdb.hpp"
#include "idastar.hpp"
#include "inverseTiles-pdb.hpp"
#include "reverseTiles-pdb.hpp"
#include "sqrtTiles-pdb.hpp"

using namespace std;

void readPDB(const std::string& pdbID,
        std::unordered_map<uint64_t, float>& htable) {
    std::ifstream f("/home/aifs1/gu/phd/research/workingPaper/"
                    "realtime-nancy/results/SlidingTilePuzzle/pdb/" +
            pdbID);
    std::string line;

    if (!f.good()) {
        std::cout << pdbID + ": PDB file not found!\n";
        return;
    }

    while (std::getline(f, line)) {
        std::stringstream ss(line);
        uint64_t tileid;
        float h;
        ss >> tileid;
        ss >> h;
        htable[tileid] = h;
    }
}

void initializePDB(std::unordered_map<uint64_t, float>& htable1,
        std::unordered_map<uint64_t, float>& htable2,
        std::string& tileType,
        std::vector<int>& pattern1,
        std::vector<int>& pattern2) {
    readPDB("61-" + tileType + ".txt", htable1);
    readPDB("62-" + tileType + ".txt", htable2);

    std::unordered_map<string, std::vector<std::vector<int>>> pdbPatterns = {
            {"heavy", {{0, 10, 11, 12, 13, 14, 15}, {0, 4, 5, 6, 7, 8, 9}}},
            {"sqrt", {{0, 10, 11, 12, 13, 14, 15}, {0, 4, 5, 6, 7, 8, 9}}},
            {"reverse", {{0, 1, 2, 3, 4, 5, 6}, {0, 7, 8, 9, 10, 11, 12}}},
            {"inverse", {{0, 1, 2, 3, 4, 5, 6}, {0, 7, 8, 9, 10, 11, 12}}}};

    pattern1 = pdbPatterns[tileType][0];
    pattern2 = pdbPatterns[tileType][1];
}

void computeTile(const char* argv[],
        istream& input,
        ostream& output,
        std::unordered_map<uint64_t, float>& htable1,
        std::unordered_map<uint64_t, float>& htable2,
        const std::vector<int>& pattern1,
        const std::vector<int>& pattern2) {
    try {
        shared_ptr<TilesPDB> tiles;

        if (strcmp(argv[2], "inverse") == 0) {
            tiles = make_shared<InverseTilesPDB>(
                    input, htable1, htable2, pattern1, pattern2);
        } else if (strcmp(argv[2], "heavy") == 0) {
            tiles = make_shared<HeavyTilesPDB>(
                    input, htable1, htable2, pattern1, pattern2);
        } else if (strcmp(argv[2], "reverse") == 0) {
            tiles = make_shared<ReverseTilesPDB>(
                    input, htable1, htable2, pattern1, pattern2);
        } else if (strcmp(argv[2], "sqrt") == 0) {
            tiles = make_shared<SqrtTilesPDB>(
                    input, htable1, htable2, pattern1, pattern2);
        } else {
            std::cout << argv[2] << "tile type not found!\n";
            throw Fatal("avaible tile types: inverse,heavy,reverse,sqrt");
        }

        SearchAlg<Tiles>* search = NULL;
        if (strcmp(argv[1], "idastar") == 0)
            search = new Idastar<Tiles>(*tiles);
        else if (strcmp(argv[1], "astar") == 0)
            search = new Astar<Tiles>(*tiles);
        else
            throw Fatal("Unknown algorithm: %s", argv[1]);

        Tiles::State init = tiles->initial();

        output << "initial heuristic " << tiles->hManhattan(init) << endl;
        double wall0 = walltime(), cpu0 = cputime();

        SolPath<Tiles> path = search->search(init);

        double wtime = walltime() - wall0, ctime = cputime() - cpu0;

        output << "total wall time " << wtime << endl;
        output << "total cpu time " << ctime << endl;
        output << "total nodes expanded " << search->expd << endl;
        output << "total nodes generated " << search->gend << endl;
        output << "solution length " << path.cost << endl;
        output << "end" << endl;

    } catch (const Fatal& f) {
        fputs(f.msg, stderr);
        fputc('\n', stderr);
        return;
    }
}

bool checkisGoodPuzzle(ifstream& input) {
    // Get the dimensions of the puzzle
    std::string line;
    getline(input, line);
    std::stringstream ss(line);
    // Get the first dimension...
    int w, h;
    ss >> w;
    ss >> h;

    // Skip the next line
    getline(input, line);

    int zeroCount = 0;
    for (int i = 0; i < w * h; i++) {
        getline(input, line);
        int tile;
        std::stringstream ss2(line);
        ss2 >> tile;

        if (tile == 0)
            zeroCount++;
    }

    return zeroCount == 1;
}

int main(int argc, const char* argv[]) {
    if (argc != 3)
        throw Fatal("Usage: tiles <algorithm> <tiletype>");

    std::unordered_map<uint64_t, float> htable1;
    std::unordered_map<uint64_t, float> htable2;

    std::vector<int> sixTiles1;
    std::vector<int> sixTiles2;

    // cout<<"initial table\n";
    std::string tileType = argv[2];
    initializePDB(htable1, htable2, tileType, sixTiles1, sixTiles2);
    // cout<<"initial finished\n";

    computeTile(argv, cin, cout, htable1, htable2, sixTiles1, sixTiles2);

    return 0;
}
