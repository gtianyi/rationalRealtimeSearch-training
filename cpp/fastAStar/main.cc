// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "tiles.hpp"
#include "idastar.hpp"
#include "astar.hpp"
#include <cstring>
#include <memory>

int main(int argc, const char *argv[]) {
    try {
        if (argc != 3)
            throw Fatal("Usage: tiles <algorithm> <tiletype>");

        shared_ptr<Tiles> tiles;

        if (strcmp(argv[2], "uniform") == 0) {
            tiles = make_shared<Tiles>(stdin);
        }
		else{
            throw Fatal("array2D only support uniform cost tiles");
		}

        SearchAlg<Tiles>* search = NULL;
        if (strcmp(argv[1], "idastar") == 0)
            search = new Idastar<Tiles>(*tiles);
        else if (strcmp(argv[1], "astar") == 0)
            search = new Astar<Tiles>(*tiles);
        else
            throw Fatal("Unknown algorithm: %s", argv[1]);

        Tiles::State init = tiles->initial();
        dfheader(stdout);
        dfpair(stdout, "initial heuristic", "%d", tiles->h(init));
        double wall0 = walltime(), cpu0 = cputime();

        std::vector<Tiles::State> path = search->search(init);

        double wtime = walltime() - wall0, ctime = cputime() - cpu0;
        dfpair(stdout, "total wall time", "%g", wtime);
        dfpair(stdout, "total cpu time", "%g", ctime);
        dfpair(stdout, "total nodes expanded", "%lu", search->expd);
        dfpair(stdout, "total nodes generated", "%lu", search->gend);
        dfpair(stdout, "solution length", "%u", (unsigned int)path.size());
        dffooter(stdout);
	} catch (const Fatal &f) {
		fputs(f.msg, stderr);
		fputc('\n', stderr);
		return 1;
	}

	return 0;
}
