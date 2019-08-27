#include "tiles-pdb.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <unordered_set>
#include <cmath>

class HeavyTilesPDB : public TilesPDB {
public:
    typedef double CostType;
    //InverseTilesPDB(FILE* f) : Tiles(f) {}
    HeavyTilesPDB(std::ifstream& input,
            std::unordered_map<uint64_t, float>& htable1,
            std::unordered_map<uint64_t, float>& htable2)
            : TilesPDB(input, htable1, htable2) {}

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        Edge<Tiles> e((double)tile, newb, s.blank);

        e.undo.h = s.h;
        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank] * (double)tile;
        s.blank = newb;
		s.tiles[newb] = 0;

        return e;
    }

    bool isgoal(const State& s) const { return s.currenth <= 0.1; }

    CostType hugeCost() const { return 1000000; }

    CostType idastarHistInterval() const { return 1; }

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
            sum += ((double)tiles[i]) * (abs(gcol - col) + abs(grow - row));
        }
        return sum;
    }
};
