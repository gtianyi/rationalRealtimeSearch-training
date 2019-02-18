#include "tiles.hpp"

class HeavyTiles : public Tiles {
public:
    using Tiles::Tiles;

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        Edge<Tiles> e(tile, newb, s.blank);

        e.undo.h = s.h;
        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank] * tile;
        s.blank = newb;

        return e;
    }
};
