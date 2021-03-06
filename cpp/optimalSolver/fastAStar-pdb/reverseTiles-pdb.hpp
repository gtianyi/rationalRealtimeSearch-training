#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "tiles-pdb.hpp"

class ReverseTilesPDB : public TilesPDB {
public:
    typedef double CostType;
    // InverseTilesPDB(FILE* f) : Tiles(f) {}
    ReverseTilesPDB(std::istream& input,
            std::unordered_map<uint64_t, float>& htable1,
            std::unordered_map<uint64_t, float>& htable2,
            const std::vector<int>& pattern1,
            const std::vector<int>& pattern2)
            : TilesPDB(input, htable1, htable2, pattern1, pattern2) {}

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        Edge<Tiles> e((16 - (double)tile), newb, s.blank);

        e.undo.h = s.h;
        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank] * (16 - (double)tile);
        s.blank = newb;
        s.tiles[newb] = 0;

        return e;
    }

    bool isgoal(const State& s) const { return s.currenth <= 0.1; }

    double h(State& s) const {
        double db1h = getPartialPDBValue(s, 1);
        double db2h = getPartialPDBValue(s, 0);

        s.patternh = db1h + db2h;

        s.currenth = std::max(s.patternh, s.h);

        return s.currenth;
    }

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

    double getPartialPDBValue(State& s, bool isDB1) const {
        auto& sixTiles = isDB1 ? sixTilesSet1 : sixTilesSet2;

        State partialState;
        for (int i = 0; i < Ntiles; i++) {
            if (sixTiles[s.tiles[i]])
                partialState.tiles[i] = s.tiles[i];
            else
                partialState.tiles[i] = 15;
        }

        partialState.blank = s.blank;

        PackedState ps;
        pack(ps, partialState);

        auto& htable = isDB1 ? htable1 : htable2;

        if (htable.find(ps.word) == htable.end()) {
            std::cout << "no pdb value found!!!\n";
            printState(partialState);
            s.patternh = -1.0;
        }

        return htable.at(ps.word);
    }
};
