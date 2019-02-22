// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>

template <class D>
class Idastar : public SearchAlg<D> {
    SolPath<D> path;
    double bound;
	static constexpr int bucketSize = 50;
    unsigned long outboundHist[bucketSize];
    double bucketInterval = 0.04;
    double incumbentCost = 1000;

public:
    Idastar(D& d) : SearchAlg<D>(d) {}

    virtual SolPath<D> search(typename D::State& root) {
		bound = this->dom.h(root);
        resetHistAndIncumbentCost();
		path.cost = 1000;

        dfrowhdr(stdout,
                "iteration",
                4,
                "number",
                "bound",
                "nodes expanded",
                "nodes generated");
        unsigned int n = 0;
        do {
            dfs(root, 0, -1);
            n++;
            dfrow(stdout,
                    "iteration",
                    "ufuu",
                    (unsigned long)n,
                    bound,
                    this->expd,
                    this->gend);
            setBound(this->expd);
            resetHistAndIncumbentCost();
        } while (path.cost == 1000);

        return path;
    }

private:
    void resetHistAndIncumbentCost() {
        for (int i = 0; i < bucketSize; ++i) {
            outboundHist[i] = 0;
        }
        incumbentCost = 1000;
    }

    void setBound(unsigned long prevExpd) {
        unsigned long accumulate = 0;

        int i = 0;

        while (i < bucketSize) {
            accumulate += outboundHist[i];
            if (accumulate >= prevExpd) {
                break;
            }
            i++;
        }

        bound += (double)i * bucketInterval;
    }

    bool dfs(typename D::State& n, double cost, int pop) {
        double f = cost + this->dom.h(n);

        if (f > incumbentCost) {
            return false;
		}

        if (f <= bound && this->dom.isgoal(n)) {
			incumbentCost = f;
			//std::cout << "incumbentCost " << f << "\n";
            path.cost =f;
			path.path.clear();
			path.path.push_back(n);
			return true;
        }

        if (f > bound) {
            double outdiff = f - bound;
            int bucket = std::min(
                    bucketSize - 1, (int)std::floor(outdiff / bucketInterval));
            outboundHist[bucket]++;
            return false;
        }

        this->expd++;
        int nops = this->dom.nops(n);
		bool oneGoal=false;
        for (int i = 0; i < nops; i++) {
            int op = this->dom.nthop(n, i);
            if (op == pop)
                continue;

            this->gend++;
            Edge<D> e = this->dom.apply(n, op);
            bool goal = dfs(n, e.cost + cost, e.pop);
			this->dom.undo(n, e);

			if(goal){
					path.path.push_back(n);
					oneGoal=true;
			}
        }

		if(oneGoal){
				return true;
		}

        return false;
	}
};

template <typename D>
constexpr int Idastar<D>::bucketSize;
