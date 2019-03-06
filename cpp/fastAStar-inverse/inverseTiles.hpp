#include "tiles.hpp"
#include "linearConflictSolver.hpp"
#include <iostream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cmath>

class InverseTiles : public Tiles {
public:
    InverseTiles(FILE* in) : Tiles(in) { initLinearConflict(); }

    struct State : public Tiles::State {
		int rows[Height][Width];
    };

	State initial() const {
        State s;
        s.blank = -1;
        int curHeight = -1;

        for (int i = 0; i < Ntiles; i++) {
            if (init[i] == 0)
                s.blank = i;
            s.tiles[i] = init[i];

            if (i % Height == 0)
                curHeight++;

            s.rows[curHeight][i % Width] = init[i];
        }

        if (s.blank < 0)
            throw Fatal("No blank tile");
        s.h = mdist(s.blank, s.tiles) + getLinearConflictByState(s);
		//return *static_cast<Tiles::State*>(&s);
		return s;
	}

    Edge<InverseTiles> apply(State& s, int newb) const {
        int tile = s.tiles[newb];

        Edge<InverseTiles> e(1.0/(double)tile, newb, s.blank);

        e.undo.h = s.h;
        e.undo.blank = s.blank;

        s.h += updateLinearConflict(s, newb);

        s.tiles[(int)s.blank] = tile;
        s.h += mdincr[tile][newb][(int)s.blank] * (1.0/(double)tile);
        s.blank = newb;
		s.tiles[newb] = 0;

        return e;
    }

    void undo(State& s, const Edge<InverseTiles>& e) const {
        s.h = e.undo.h;
        s.tiles[(int)s.blank] = s.tiles[(int)e.undo.blank];
        s.blank = e.undo.blank;
        s.tiles[(int)e.undo.blank] = 0;
    }

    // unpack unpacks the packed state s into the state dst.
    void unpack(State& dst, PackedState s) const {
        dst.h = 0;
        dst.blank = -1;

        int curHeight = Height - 1, curWidth = Width - 1;

        for (int i = Ntiles - 1; i >= 0; i--) {
            int t = s.word & 0xF;
            s.word >>= 4;
            dst.tiles[i] = t;
            if (t == 0)
                dst.blank = i;
            else
                dst.h += md[t][i] * (1.0 / (double)t);

			if (i % Height == 0)
                curHeight--;
            if (i % Width == 0)
                curWidth--;

            dst.rows[curHeight][i % Width] = t;
        }

        dst.h += getLinearConflictByState(dst);

        assert(dst.blank >= 0);
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

    void initLinearConflict() {
        std::vector<std::vector<int>> allRow;
		LinearConflictSolver solver;

        for (int t1 = 0; t1 < Ntiles; t1++) {
            for (int t2 = 0; t2 < Ntiles; t2++) {
                for (int t3 = 0; t3 < Ntiles; t3++) {
                    for (int t4 = 0; t4 < Ntiles; t4++) {
                        set<int> dupSet;
                        dupSet.insert(t1);
                        dupSet.insert(t2);
                        dupSet.insert(t3);
                        dupSet.insert(t4);

                        if (dupSet.size() == 4) {
                            std::vector<int> tiles;
                            tiles.push_back(t1);
                            tiles.push_back(t2);
                            tiles.push_back(t3);
                            tiles.push_back(t4);

                            allRow.push_back(tiles);
                        }
                    }
                }
            }
        }

        assert(allRow.size() == 43680);

        for (int rc = 0; rc < Height; rc++) {
            int rowValueTable[Ntiles] = {0};

            for (int i = 0; i < Width; i++) {
                int face = i + Width * rc;
                rowValueTable[face] = 1;
            }

            rowValueTable[0] = 0;

            for (auto& row : allRow) {
                std::vector<int> goalRowFaces;
                for (auto face : row) {
                    if (rowValueTable[face]) {
                        goalRowFaces.push_back(face);
                    }
                }

                if (goalRowFaces.size() > 1) {
                    double conflict = solver.solve(goalRowFaces);
                    int v = getCompactIntByArray(&row[0], Width);
                    rowlinearConflict[rc][v] = conflict;
                }
            }

            if (rc > 0)
                assert(rowlinearConflict[rc].size() == 10680);
        }
    }

    double getConflict(std::vector<int>& v) {
        std::vector<int> swapElement;
        for (int i = 1; i < v.size(); i++) {
            if (v[i - 1] > v[i] && v[i] != 0) {
                swapElement.push_back(v[i - 1]);
            }
        }

        double ret = 0.0;
        for (auto e : swapElement) {
            if (e != 0) {
                ret += 1.0 / (double)e;
            }
        }

        ret *= 2;

        return ret;
    }

    double getLinearConflictByState(State& s) const {
        double ret = 0.0;

        for (int i = 0; i < Height; i++) {
            ret += getLinearConflictByArray(s.rows[i], i);
        }

        return ret;
    }

    double getLinearConflictByArray(int* v, int rcindex) const {
        double ret = 0;

        int value = getCompactIntByArray(v, Width);

        if (rowlinearConflict.at(rcindex).find(value) !=
                rowlinearConflict.at(rcindex).end()) {
            ret = rowlinearConflict.at(rcindex).at(value);
        }

        return ret;
    }

    double updateLinearConflict(State& s, int newb) const {
        int row1 = s.blank / Height;
        int row2 = newb / Height;

        int col1 = s.blank % Width;
        int col2 = newb % Width;

        double old_rowLC, new_rowLC;

        if (row1 != row2) {
            old_rowLC += getLinearConflictByArray(s.rows[row1], row1);
            old_rowLC += getLinearConflictByArray(s.rows[row2], row2);
        }

        // update rows and cols in tile
        s.rows[row1][col1] = s.rows[row2][col2];

        s.rows[row2][col2] = 0;

        if (row1 != row2) {
            new_rowLC += getLinearConflictByArray(s.rows[row1], row1);
            new_rowLC += getLinearConflictByArray(s.rows[row2], row2);
        }

        double ret = -old_rowLC + new_rowLC;

        return ret;
    }

    std::unordered_map<int, std::unordered_map<int, double>> rowlinearConflict;
};
