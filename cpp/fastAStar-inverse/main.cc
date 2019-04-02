// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "inverseTiles-pdb.hpp"
#include "idastar.hpp"
#include "astar.hpp"
#include <cstring>
#include <memory>

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
				std::unordered_map<uint64_t, float>& htable2){
		readPDB("61.txt",htable1);
		readPDB("62.txt",htable2);
		
}

void computeTile(const char* argv[],
        ifstream& input,
        ofstream& output,
        std::unordered_map<uint64_t, float>& htable1,
        std::unordered_map<uint64_t, float>& htable2) {
    try {
        shared_ptr<Tiles> tiles;

        if (strcmp(argv[2], "inverse") == 0) {
            tiles = make_shared<InverseTilesPDB>(input, htable1, htable2);
        } else {
            throw Fatal("Wrong tile type: inverse");
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

        output.close();
    } catch (const Fatal& f) {
        fputs(f.msg, stderr);
        fputc('\n', stderr);
        return;
    }
}

bool checkisGoodPuzzle(ifstream& input){
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
    if (argc != 5)
        throw Fatal("Usage: tiles <algorithm> <tiletype> <start instance> "
                    "<number of instance to run>");

    std::unordered_map<uint64_t, float> htable1;
    std::unordered_map<uint64_t, float> htable2;

	//cout<<"initial table\n";
    initializePDB(htable1, htable2);
	//cout<<"initial finished\n";

    int startInstance = stoi(argv[3]);
    int numberOfInstance = stoi(argv[4]);

    for (int i = 0; i < numberOfInstance; i++) {
        int instanceID = startInstance + i;
        string resultFile = "/home/aifs1/gu/phd/research/workingPaper/"
                            "realtime-nancy/results/SlidingTilePuzzle/"
                            "sampleData/" +
                string(argv[2]) + "/" + to_string(instanceID) + ".txt";
        ifstream checkRetExist(resultFile);
        if (checkRetExist.good()) {
            continue;
        }

        ofstream output(resultFile);

        string inputFile = "/home/aifs1/gu/phd/research/workingPaper/"
                           "realtime-nancy/results/SlidingTilePuzzle/"
                           "sampleProblem/" +
                string(argv[2]) + "/" + to_string(instanceID) + ".st";

        ifstream input(inputFile);
        ifstream inputcheck(inputFile);

        if (!checkisGoodPuzzle(inputcheck))
            continue;

        // cout << "start " << instanceID << endl;
        if (!input.good())
            continue;

        computeTile(argv, input, output, htable1, htable2);
    }

    return 0;
}


