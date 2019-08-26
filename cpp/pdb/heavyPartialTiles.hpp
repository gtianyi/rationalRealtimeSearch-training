/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: partialTiles.hpp
 * description: partial Tile struct for fifteen puzzle
 * date: March-25-2019
 */

#include "partialTiles.hpp"

using namespace std;

class HeavyPartialTiles : public PartialTiles {
public:
    HeavyPartialTiles(int patternSize) :PartialTiles(patternSize){};
    HeavyPartialTiles() :PartialTiles(){};

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        float cost = 0;

        if (tile != 15)
            cost = (float)tile;

        Edge<Tiles> e(cost, newb, s.blank);

        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.blank = newb;
		s.tiles[newb] = 0;

        return e;
    }

    string getType() const { return "heavy"; }
};
