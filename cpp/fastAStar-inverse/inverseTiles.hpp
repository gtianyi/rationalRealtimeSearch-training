#include "tiles.hpp"
#include <iostream>

class InverseTiles : public Tiles {
public:
    using Tiles::Tiles;

    State initial() const {
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
        return s;
	}

    Edge<Tiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        Edge<Tiles> e(1.0/(double)tile, newb, s.blank);

        e.undo.h = s.h;
        e.undo.blank = s.blank;

        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank] * (1.0/(double)tile);
        s.blank = newb;

        return e;
    }

	// unpack unpacks the packed state s into the state dst.
	void unpack(State &dst, PackedState s) const {
		dst.h = 0;
		dst.blank = -1;
		for (int i = Ntiles - 1; i >= 0; i--) {
			int t = s.word & 0xF;
			s.word >>= 4;
			dst.tiles[i] = t;
			if (t == 0)
				dst.blank = i;
			else
				dst.h += md[t][i]*(1.0/(double)t);
		}
		assert (dst.blank >= 0);
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
            sum += (1.0/(double)tiles[i]) * (abs(gcol - col) + abs(grow - row));
        }
        return sum;
    }
};
