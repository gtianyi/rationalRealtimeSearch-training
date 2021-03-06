// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "fatal.hpp"
#include "hashtbl.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <stdint.h>
#include <iostream>

struct Tiles {
    enum {
        Width = 4,
        Height = 4,
        Ntiles = Width * Height,
    };

    struct State {
        int tiles[Ntiles];
        int blank;
        double h;
		double patternh;
		double currenth;
    };

    struct PackedState {
        uint64_t word;

        unsigned long hash() const { return word; }

        bool eq(const PackedState& h) const { return word == h.word; }

		//int blank;
    };

    // Tiles constructs a new instance by reading
    // the initial state from the given file which is
    // expected to be in Wheeler's tiles instance
    // format.
    Tiles(FILE*);

	//constructor for partial tile
	Tiles();

    virtual State initial() {
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

    virtual double h(State& s) const { return s.h; }

    double hManhattan(State& s) const { return s.h; }

    virtual bool isgoal(const State& s) const { return s.h <= 0.05; }

    void printState(const State& s) const {
        for (int i = 0; i < Ntiles; i++) {
            std::cout << s.tiles[i] << " ";
        }
        std::cout << s.blank;
        std::cout << "\n";
    }

    int nops(const State& s) const { return optab[(int)s.blank].n; }

    int nthop(const State& s, int n) const {
        return optab[(int)s.blank].ops[n];
    }

    struct Undo {
        double h;
        int blank;
    };

    virtual Edge<Tiles> apply(State& s, int newb) const {
        Edge<Tiles> e(1, newb, s.blank);
        e.undo.h = s.h;
        e.undo.blank = s.blank;

        int tile = s.tiles[newb];
        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank];
        s.blank = newb;

        return e;
	}

	virtual void undo(State &s, const Edge<Tiles> &e) const {
		s.h = e.undo.h;
		s.tiles[(int) s.blank] = s.tiles[(int) e.undo.blank];
		s.blank = e.undo.blank;
        s.tiles[(int)e.undo.blank] = 0;
    }

	// pack packes state s into the packed state dst.
	virtual void pack(PackedState &dst, State &s) const {

		dst.word = 0;	// to make g++ shut up about uninitialized usage.
		s.tiles[(int) s.blank] = 0;
		for (int i = 0; i < Ntiles; i++)
			dst.word = (dst.word << 4) | s.tiles[i];
	}

	// unpack unpacks the packed state s into the state dst.
	virtual void unpack(State &dst, PackedState s) const {
		dst.h = 0;
		dst.blank = -1;
		for (int i = Ntiles - 1; i >= 0; i--) {
			int t = s.word & 0xF;
			s.word >>= 4;
			dst.tiles[i] = t;
			if (t == 0)
				dst.blank = i;
			else
				dst.h += md[t][i];
		}
		assert (dst.blank >= 0);
	}

protected:
    // mdist returns the Manhattan distance of the given tile array.
    virtual double mdist(int blank, int tiles[]) const {
        int sum = 0;
        for (int i = 0; i < Ntiles; i++) {
            if (i == blank)
                continue;
            int row = i / Width, col = i % Width;
            int grow = tiles[i] / Width, gcol = tiles[i] % Width;
            sum += abs(gcol - col) + abs(grow - row);
        }
        return sum;
	}

	// initmd initializes the md and mdincr tables.
	void initmd();

	// initoptob initializes the operator table, optab.
	void initoptab();

	// init is the initial tile positions.
	int init[Ntiles];

	// md is indexed by tile and location.  Each entry
	int md[Ntiles][Ntiles];

	// mdincr is indexed by tile, source location, and
	// destination location.  Each entry is the change
	// in Manhattan distance for that particular move.
	int mdincr[Ntiles][Ntiles][Ntiles];

	// optab is indexed by the blank position.  Each
	// entry is a description of the possible next
	// blank positions.
	struct { int n, ops[4]; } optab[Ntiles];
};
