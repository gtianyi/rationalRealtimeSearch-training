/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: partialTiles.hpp
 * description: partial Tile struct for fifteen puzzle
 * date: March-25-2019
 */

#include "partialTiles.hpp"

using namespace std;

class ReversePartialTiles : public PartialTiles {
public:
    ReversePartialTiles(int patternSizewithID) : PartialTiles() {
        generatePattern(patternSizewithID);
    };

    ReversePartialTiles() : PartialTiles(){};

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        float cost = 0;

        //notice we use 15 for all covered tile
        //because we want pdb to be mostly high cost tiles
        if (tile != 15)
            cost = (16.0 - (float)tile);

        Edge<Tiles> e(cost, newb, s.blank);

        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.blank = newb;
        s.tiles[newb] = 0;

        return e;
    }

    string getType() const { return "reverse"; }

    void generatePattern(int patternSizewithID) {
        init[0] = 0;
        if (patternSizewithID == 61) {
            int sixTiles[6] = {1, 2, 3, 4, 5, 6};
            int converedTiles[9] = {7, 8, 9, 10, 11, 12, 13, 14, 15};

            for (int i = 0; i <= 5; i++) {
                init[sixTiles[i]] = sixTiles[i];
            }

            for (int i = 0; i <= 8; i++) {
                init[converedTiles[i]] = 15;
            }
        } else if (patternSizewithID == 62) {
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
};
