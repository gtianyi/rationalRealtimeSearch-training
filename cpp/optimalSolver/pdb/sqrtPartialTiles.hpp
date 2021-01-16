/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: sqrtPartialTiles.hpp
 * description: partial Tile struct for sqrt fifteen puzzle
 * date: Jan-16-2021
 */

#include "partialTiles.hpp"
#include <cmath>

using namespace std;

class SqrtPartialTiles : public PartialTiles {
public:
    SqrtPartialTiles(int patternSize) : PartialTiles(){
        generatePattern(patternSize);
    };
    SqrtPartialTiles() :PartialTiles(){};

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        float cost = 0;

        // this has to be match with covered cell mark
        // here we use 1 as the cover
        // because we want mostly high cost tils in 6-partial puzzle
        if (tile != 1)
            cost = sqrt((float)tile);

        Edge<Tiles> e(cost, newb, s.blank);

        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.blank = newb;
		s.tiles[newb] = 0;

        return e;
    }

    string getType() const { return "sqrt"; }

    void generatePattern(int patternSizewithID) {
        init[0] = 0;
        if (patternSizewithID == 61) {
            int sixTiles[6] = {10, 11, 12, 13, 14, 15};
            int converedTiles[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

            for (int i = 0; i <= 5; i++) {
                init[sixTiles[i]] = sixTiles[i];
            }

            for (int i = 0; i <= 8; i++) {
                init[converedTiles[i]] = 1;
            }
        } else if (patternSizewithID == 62) {
            int sixTiles[6] = {4, 5, 6, 7, 8, 9};
            int converedTiles[9] = {1, 2, 3, 10, 11, 12, 13, 14, 15};

            for (int i = 0; i <= 5; i++) {
                init[sixTiles[i]] = sixTiles[i];
            }

            for (int i = 0; i <= 8; i++) {
                init[converedTiles[i]] = 1;
            }
        } else {
            cout << "wrong pattern size\n";
        }

        initoptab();
    }
};
