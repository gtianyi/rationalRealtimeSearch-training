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
    typedef int CostType;
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

    bool isgoal(const State& s) const { return s.currenth == 0.0; }

    struct IDAStarTools {
        CostType bound;
        static constexpr int bucketSize = 50;
        unsigned long outboundHist[bucketSize];
        double bucketInterval = 10;

        void updateBound(CostType b) { bound = b; }
        double getBound() const { return bound; }

        void resetHistAndIncumbentCost(CostType& incumbentCost) {
            for (int i = 0; i < bucketSize; ++i) {
                outboundHist[i] = 0;
            }
            incumbentCost = hugeCost();
        }

        void setBound(unsigned long prevExpd) {
            unsigned long accumulate = 0;

            int i = 0;

            // std::cout << "bucket accu: ";
            while (i < bucketSize) {
                accumulate += outboundHist[i];
                if (accumulate >= prevExpd) {
                    break;
                }
                // std::cout << "i: " << i << " accu: " << outboundHist[i] << "
                // ";
                i++;
            }
            // std::cout << "\n";

            bound += (CostType)i * bucketInterval;
        }

        CostType hugeCost() { return 1000000; }

        void updateHist(CostType f) {
            CostType outdiff = f - bound;
            int bucket = (int)std::floor(outdiff / bucketInterval);
            if (bucket < bucketSize)
                outboundHist[bucket]++;
        }
    };

    IDAStarTools idastartools;

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
