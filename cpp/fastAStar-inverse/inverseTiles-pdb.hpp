#include "tiles.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>

class InverseTilesPDB : public Tiles {
public:
    InverseTilesPDB(FILE* f) : Tiles(f) {}

    State initial() {
        State s;
        s.blank = -1;
        for (int i = 0; i < Ntiles; i++) {
            if (init[i] == 0)
                s.blank = i;
            s.tiles[i] = init[i];
        }
        if (s.blank < 0)
            throw Fatal("No blank tile");

        s.h = mdist(s.blank, s.tiles);

        initialPDB();

        return s;
	}

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        Edge<Tiles> e(1.0/(double)tile, newb, s.blank);

        e.undo.h = s.h;
        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank] * (1.0 / (double)tile);
        s.blank = newb;
		s.tiles[newb] = 0;

        return e;
    }

    void undo(State& s, const Edge<Tiles>& e) const {
        s.h = e.undo.h;
        s.tiles[(int)s.blank] = s.tiles[(int)e.undo.blank];
        s.blank = e.undo.blank;
        s.tiles[(int)e.undo.blank] = 0;
    }


    bool isgoal(const State& s) const { return s.currenth <= 0.05; }

protected:
    // mdist returns the Manhattan distance of the given tile array.
    // this only work for initialization
    double mdist(int blank, int tiles[]) const {
        double sum = 0;
        for (int i = 0; i < Ntiles; i++) {
            if (i == blank)
                continue;
            int row = i / Width, col = i % Width;
            int grow = tiles[i] / Width, gcol = tiles[i] % Width;
            sum += (1.0/(double)tiles[i]) * (abs(gcol - col) + abs(grow - row));
        }
        return sum;
    }

    void initialPDB() {
        std::ifstream f("/home/aifs1/gu/phd/research/workingPaper/realtime-nancy/results/SlidingTilePuzzle/pdb/6.txt");
        std::string line;

		if(!f.good())
            std::cout << "PDB file not found!\n";

        while (std::getline(f, line)) {
            std::stringstream ss(line);
			uint64_t tileid;
			float h;
			ss>>tileid;
			ss>>h;
			htable[tileid] = h;
        }
    }

    double h(State& s) const {
        State partialState;
        for (int i = 0; i < Ntiles; i++) {
            if (s.tiles[i] <= 6)
                partialState.tiles[i] = s.tiles[i];
            else
                partialState.tiles[i] = 15;
        }

		partialState.blank = s.blank;


        PackedState ps;
        pack(ps,partialState);

        if (htable.find(ps.word) == htable.end()) {
            std::cout << "no pdb value found!!!\n";
            printState(partialState);
            s.patternh = 999.0;
        }

        s.patternh = htable.at(ps.word);
        s.currenth = std::max(s.patternh, s.h);

        return s.currenth;
    }

    std::unordered_map<uint64_t, float> htable;
};
