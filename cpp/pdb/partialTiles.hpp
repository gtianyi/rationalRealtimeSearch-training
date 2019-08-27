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
    virtual void generatePattern(int patternSizewithID) = 0;
};
