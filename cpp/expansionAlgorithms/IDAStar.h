#pragma once
#include <unordered_map>
#include <functional>
#include "../utility/PriorityQueue.h"
#include "../utility/SearchResultContainer.h"
#include <cmath>
#include <iomanip>

using namespace std;

template <class Domain, class Node>
class IDAStar {
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
    IDAStar(Domain& domain, string sorting)
            : domain(domain), sortingFunction(sorting) {}

    double search(Node* root, SearchResultContainer& _res) {
        res = _res;

        bound = root->getHValue();

		resetHistAndIncumbentCost();

        solutionCost = hugeCost;

        unsigned int n = 0;

        do {
            dfs(root, NULL);
            n++;
            cout << "iter: " << std::setw(3) << n 
				 << " bound: " << std::setw(5) << bound
                 << " expd: "  << std::setw(8) << res.nodesExpanded
                 << " gend: "  << std::setw(8) << res.nodesGenerated << endl;

            setBound((int)std::pow(2, n));
            resetHistAndIncumbentCost();
        } while (solutionCost >=  hugeCost);

        return solutionCost;
    }


    double search(PriorityQueue<Node*>& open,
            unordered_map<State, Node*, Hash>& closed,
            std::function<bool(Node*,
                    unordered_map<State, Node*, Hash>&,
                    PriorityQueue<Node*>&)> duplicateDetection,
            SearchResultContainer& res) {
        sortOpen(open);

        // Expand until find the goal
        while (!open.empty()) {
            // Pop lowest fhat-value off open
            Node* cur = open.top();

            // Check if current node is goal
            if (domain.isGoal(cur->getState())) {
                return cur->getFValue();
            }

            res.nodesExpanded++;

            open.pop();
            cur->close();

            vector<State> children = domain.successors(cur->getState());
            res.nodesGenerated += children.size();

            for (State child : children) {
                Node* childNode =
                        new Node(cur->getGValue() + domain.getEdgeCost(child),
                                 domain.heuristic(child),
                                child,
                                cur);

                bool dup = duplicateDetection(childNode, closed, open);

                // Duplicate detection
                if (!dup) {
                    open.push(childNode);
                    closed[child] = childNode;
                } else
                    delete childNode;
            }
        }
	}

private:
	void resetHistAndIncumbentCost() {
        for (int i = 0; i < bucketSize; ++i) {
            outboundHist[i] = 0;
        }
        incumbentCost = hugeCost;
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

        bound += (Cost)i * bucketInterval;
    }

    void updateHist(Cost f) {
        Cost outdiff = f - bound;
        int bucket = (int)std::floor(outdiff / bucketInterval);
        if (bucket < bucketSize)
            outboundHist[bucket]++;
    }

    bool dfs(Node* n, Node* p) {
        Cost f = n->getFValue();

        if (f >= incumbentCost) {
            return false;
		}

        if (f <= bound && domain.isGoal(n->getState())) {
			incumbentCost = f;
			//std::cout << "incumbentCost " << f << "\n";
            solutionCost =f;
			return true;
        }

        if (f > bound) {
            updateHist(f);
            return false;
        }

        res.nodesExpanded++;

        vector<State> children = domain.successors(n->getState());
        res.nodesGenerated += children.size();

        bool goal = false;
        for (State child : children) {
            Node* childNode =
                    new Node(n->getGValue() + domain.getEdgeCost(child),
                            domain.heuristic(child),
                            child,
                            n);

			//prevent circle, but not sure if this is 100% enough
            if (p && childNode->getState() == p->getState())
                continue;

            if (dfs(childNode, n)) {
                goal = true;
            }

            delete childNode;
        }

        return goal;
    }

protected:
	Domain & domain;
	string sortingFunction;

	Cost solutionCost;
    Cost  incumbentCost;

    Cost  bound;
    static constexpr int bucketSize = 50;
    unsigned long outboundHist[bucketSize];
    double bucketInterval = 1;
    double hugeCost = 10000;
    SearchResultContainer res;
};
