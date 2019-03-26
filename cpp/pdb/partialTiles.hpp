/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: partialTiles.hpp
 * description: partial Tile struct for fifteen puzzle
 * date: March-25-2019
 */

#include "../fastAStar-inverse/tiles.hpp"

using namespace std;

class PartialTiles : public Tiles {
public:
    PartialTiles(int patternSize) :Tiles() {
        init[0] = 0;

        if (patternSize == 7) {
            for (int i = 1; i <= 7; i++) {
                init[i] = i;
            }

            for (int i = 8; i <= 15; i++) {
                init[i] = 16;
            }
        } else if (patternSize == 8) {
            for (int i = 1; i <= 7; i++) {
                init[i] = 16;
            }

            for (int i = 8; i <= 15; i++) {
                init[i] = i;
            }
        } else {
            cout << "wrong pattern size\n";
        }

        initoptab();
    }

    State initial() const {
        State s;
        s.blank = -1;
        for (int i = 0; i < Ntiles; i++) {
            if (init[i] == 0)
                s.blank = i;
            s.tiles[i] = init[i];
        }
        return s;
    }

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        float cost = 0;

        if (tile != 16)
            cost = 1.0 / (float)tile;

        Edge<Tiles> e(cost, newb, s.blank);

        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.blank = newb;
		s.tiles[newb] = 0;

        return e;
    }
};
