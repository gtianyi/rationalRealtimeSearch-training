#pragma once
/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: partialTiles.hpp
 * description: partial Tile struct for fifteen puzzle
 * date: March-25-2019
 */


#include "../fastAStar-pdb/tiles.hpp"

using namespace std;

class PartialTiles : public Tiles {
public:
    PartialTiles(int patternSize) :Tiles() {
        init[0] = 0;

        if (patternSize == 61) {
            int sixTiles[6] = {1, 2, 3, 4, 5, 6};
            int converedTiles[9] = {7, 8, 9, 10, 11, 12, 13, 14, 15};

            for (int i = 0; i <= 5; i++) {
                init[sixTiles[i]] = sixTiles[i];
            }

            for (int i = 0; i <= 8; i++) {
                init[converedTiles[i]] = 15;
            }
        } else if (patternSize == 62) {
            int sixTiles[6] = {7, 8, 9, 10, 11, 12};
            int converedTiles[9] = {1, 2, 3, 4, 5, 6, 13, 14, 15};

            for (int i = 0; i <= 5; i++) {
                init[sixTiles[i]] = sixTiles[i];
            }

            for (int i = 0; i <= 8; i++) {
                init[converedTiles[i]] = 15;
            }
        } else {
            cout << "wrong pattern size\n";
        }

        initoptab();
    }

    PartialTiles() : Tiles() {}

    State initial() const {
        State s;
        s.blank = -1;
        for (int i = 0; i < Ntiles; i++) {
            if (init[i] == 0)
                s.blank = i;
            s.tiles[i] = init[i];
            cout << s.tiles[i] << endl;
        }
        return s;
    }

    virtual Edge<Tiles> apply(State& s, int newb) const = 0;
    virtual string getType() const = 0;
};
