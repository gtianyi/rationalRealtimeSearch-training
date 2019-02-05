#pragma once
#include <unordered_map>
#include <functional>
#include "../utility/PriorityQueue.h"
#include "../utility/WAStarResultContainer.h"

using namespace std;

template <class Domain, class Node>
class WAStar {
	typedef typename Domain::State State;
	typedef typename Domain::Cost Cost;
	typedef typename Domain::HashState Hash;

public:
    WAStar(Domain& domain, float weight, string sorting)
            : domain(domain), weight(weight), sortingFunction(sorting) {}

    void expand(PriorityQueue<Node*>& open,
            unordered_map<State, Node*, Hash>& closed,
            std::function<bool(Node*,
                    unordered_map<State, Node*, Hash>&,
                    PriorityQueue<Node*>&)> duplicateDetection,
            WAStarResultContainer& res) {
        sortOpen(open);

        // Expand until find the goal
        while (!open.empty()) {
            // Pop lowest fhat-value off open
            Node* cur = open.top();

            // Check if current node is goal
            if (domain.isGoal(cur->getState())) {
                return;
            }

            res.nodesExpanded++;

            open.pop();
            cur->close();

            vector<State> children = domain.successors(cur->getState());
            res.nodesGenerated += children.size();

            for (State child : children) {
                Node* childNode =
                        new Node(cur->getGValue() + domain.getEdgeCost(child),
                                weight * domain.heuristic(child),
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
    void sortOpen(PriorityQueue<Node*>& open) {
        open.swapComparator(Node::compareNodesF);
    }

protected:
	Domain & domain;
	float weight;
	string sortingFunction;
};
