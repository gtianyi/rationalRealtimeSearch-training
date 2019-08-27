// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "search.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>

template <class D>
class Idastar : public SearchAlg<D> {
    typedef typename D::CostType CostType;
    SolPath<D> path;
    CostType incumbentCost;

public:
    Idastar(D& d) : SearchAlg<D>(d) {
        incumbentCost = d.idastartools.hugetCost();
    }

    virtual SolPath<D> search(typename D::State& root) {
		this->dom.idastartools.updateBound(this->dom.h(root));
        this->dom.idastartools.resetHistAndIncumbentCost(incumbentCost);
        path.cost = this->dom.idastartools.hugetCost();

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
                    this->dom.idastartools.getBound(),
                    this->expd,
                    this->gend);

            this->dom.idastartools.setBound((int)std::pow(4, n));
            this->dom.idastartools.resetHistAndIncumbentCost(incumbentCost);
        } while (path.cost == this->dom.idastartools.hugetCost());

        return path;
    }

private:
    bool dfs(typename D::State& n, CostType cost, int pop) {
        CostType f = cost + this->dom.h(n);

        if (f > incumbentCost) {
            return false;
		}

        if (f <= this->dom.idastartools.getBound() && this->dom.isgoal(n)) {
			incumbentCost = f;
			//std::cout << "incumbentCost " << f << "\n";
            path.cost =f;
			path.path.clear();
			path.path.push_back(n);
			return true;
        }

        if (f > this->dom.getBound()) {
            this->dom.idastartools.updateHist(f);
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
