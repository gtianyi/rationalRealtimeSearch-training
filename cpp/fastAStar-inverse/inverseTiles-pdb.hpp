#include "tiles.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <unordered_set>

class InverseTilesPDB : public Tiles {
public:
    //InverseTilesPDB(FILE* f) : Tiles(f) {}
    InverseTilesPDB(std::ifstream& input,
            std::unordered_map<uint64_t, float>& htable1,
            std::unordered_map<uint64_t, float>& htable2)
            : Tiles(), htable1(htable1), htable2(htable2) {
        // Get the dimensions of the puzzle
        std::string line;
        getline(input, line);
        std::stringstream ss(line);
        // Get the first dimension...
        int w, h;
        ss >> w;
        ss >> h;

        if (w != Width && h != Height)
            throw Fatal("Width and height instance/compiler option mismatch");

        // Skip the next line
        getline(input, line);

        for (int i = 0; i < w * h; i++) {
            getline(input, line);
            int tile;
            std::stringstream ss2(line);
            ss2 >> tile;

            init[i] = tile;
        }

        // Skip the next line
        getline(input, line);

        for (int i = 0; i < w * h; i++) {
            getline(input, line);
            int tile;
            std::stringstream ss2(line);
            ss2 >> tile;

            if (i != tile)
                throw Fatal("Non-canonical goal positions");
        }

		initmd();
		initoptab();
    }

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
	   initializeTileSet("61.txt") ;
	   initializeTileSet("62.txt") ;
    }

    void initializeTileSet(const std::string& pdbID) {
        
		//initialize tileSet
		auto& tiles = pdbID == "61.txt" ? sixTiles1 : sixTiles2;
		auto& tileSets = pdbID == "61.txt" ? sixTilesSet1 : sixTilesSet2;

        for (auto i : tiles) {
            tileSets[i] = 1;
		}
    }

    double h(State& s) const {
		double db1h = getPartialPDBValue(s, 1);
		double db2h = getPartialPDBValue(s, 0);

		s.patternh = db1h + db2h;

        s.currenth = std::max(s.patternh, s.h);

        return s.currenth;
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
        pack(ps,partialState);

        auto& htable = isDB1 ? htable1 : htable2;

        if (htable.find(ps.word) == htable.end()) {
            std::cout << "no pdb value found!!!\n";
            printState(partialState);
            s.patternh = -1.0;
        }

		return htable.at(ps.word);
	}

    std::unordered_map<uint64_t, float>& htable1;
    std::unordered_map<uint64_t, float>& htable2;
    const int sixTiles1[7] = {0, 1, 2, 3, 4, 5, 6};
    const int sixTiles2[7] = {0, 7, 8, 9, 10, 11, 12};
    int sixTilesSet1[16] = {0};
    int sixTilesSet2[16] = {0};
};
