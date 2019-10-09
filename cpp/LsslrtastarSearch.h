#pragma once
#include "RealTimeSearch.h"

template <class Domain>
class LssLRTAStarSearch : public RealTimeSearch<Domain> {
    typedef typename RealTimeSearch<Domain>::Node Node;
    typedef typename RealTimeSearch<Domain>::State State;
    typedef typename RealTimeSearch<Domain>::Hash Hash;

public:
    LssLRTAStarSearch(Domain& domain,
            string expansionModule,
            string learningModule,
            string decisionModule,
            double lookahead,
            double k = 1,
            string belief = "normal")
            : RealTimeSearch<Domain>(domain,
                      expansionModule,
                      learningModule,
                      decisionModule,
                      lookahead,
                      k,
                      belief) {}

   ResultContainer search() {
        this->domain.initialize(this->expansionPolicy, this->lookahead);

        ResultContainer res;

        // Get the start node
        Node* start = new Node(0,
                this->domain.heuristic(this->domain.getStartState()),
                this->domain.distance(this->domain.getStartState()),
                this->domain.distanceErr(this->domain.getStartState()),
                this->domain.epsilonHGlobal(),
                this->domain.epsilonDGlobal(),
                this->domain.getStartState(),
                NULL,
                -1);

        clock_t startTime = clock();
        int iterationCounter = 1;

        while (1) {
            // Check if a goal has been reached
            if (this->domain.isGoal(start->getState())) {
                // Calculate path cost and return solution
                calculateCost(start, res);

                res.totalCpuTime = double(clock() - startTime) /
                        CLOCKS_PER_SEC / iterationCounter;

                return res;
            }

            restartLists(start);

            // Exploration Phase
            this->domain.updateEpsilons();

            // First, generate the top-level actions
            generateTopLevelActions(start, res);

            // Expand some nodes until expnasion limit
            this->expansionAlgo->expand(this->open, this->closed, this->tlas, this->duplicateDetection, res);

            // Check if this is a dead end 
			// or reach the lookahead limit
            if (this->open.empty()) {
                res.totalCpuTime = double(clock() - startTime) /
                        CLOCKS_PER_SEC / iterationCounter;
                break;
            }

			recordVisited();

            //  Learning Phase
            this->learningAlgo->learn(this->open, this->closed);

            // Decision-making Phase
            start = this->decisionAlgo->backup(this->open, this->tlas, start);

            iterationCounter++;
        }

		return res;
    }

    void dumpClosedList(ofstream& out) {
        for (typename unordered_map<State, Node*, Hash>::iterator it =
                        allVisitedStates.begin();
                it != allVisitedStates.end();
                it++) {
            out << it->first;
            out << it->second->getHValue() << " " << it->second->getDValue()
                << " " << it->first.key() << endl;
        }
    }

private:
    void recordVisited() {
        // keep the copy of the first time we see it
        for (typename unordered_map<State, Node*, Hash>::iterator it =
                        this->closed.begin();
                it != this->closed.end();
                it++)
            if (allVisitedStates.find(it->first) != allVisitedStates.end()) {
                allVisitedStates[it->first] = it->second;
            } 
	}

	unordered_map<State, Node*, Hash> allVisitedStates;
};
