#pragma once
#include "RealTimeSearch.h"
#include "SuboptimalSearchBase.h"

template <class Domain>
class LssLRTAStarSearch : public RealTimeSearch<Domain>, public SuboptimalSearch {
    typedef typename RealTimeSearch<Domain>::Node Node;
	typedef typename RealTimeSearch<Domain>::State State;
    typedef typename RealTimeSearch<Domain>::Cost Cost;
    typedef typename RealTimeSearch<Domain>::Hash Hash;
    typedef typename RealTimeSearch<Domain>::TopLevelAction TopLevelAction;

    struct DumpNode {
        Cost h;
        Cost d;

        DumpNode(Node* n) {
            h = n->getHValue();
            d = n->getDValue();
        }
	};

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

    SuboptSearchResultContainer subOptSearch() {
        this->domain.initialize(this->expansionPolicy, this->lookahead);

        SuboptSearchResultContainer res;

        auto d = this->domain.distance(this->domain.getStartState());
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

        recordGenerated(start);

        clock_t startTime = clock();
        int iterationCounter = 1;

        while (1) {
            // Check if a goal has been reached
            if (this->domain.isGoal(start->getState())) {
                // Calculate path cost and return solution
                this->calculateCost(start, res);

                res.totalCpuTime = double(clock() - startTime) /
                        CLOCKS_PER_SEC / iterationCounter;

                return res;
            }

            this->restartLists(start);

            // Exploration Phase
            this->domain.updateEpsilons();

            // First, generate the top-level actions
            this->generateTopLevelActions(start, res);

            // Expand some nodes until expnasion limit
            this->expansionAlgo->expand(this->open,
                    this->closed,
                    this->tlas,
                    duplicateDetection_recordGenerated,
                    res);

            // Check if this is a dead end
            // or reach the lookahead limit
            if (this->open.empty()) {
                res.totalCpuTime = double(clock() - startTime) /
                        CLOCKS_PER_SEC / iterationCounter;
                break;
            }

            //  Learning Phase
            this->learningAlgo->learn(this->open, this->closed);

            // Decision-making Phase
            start = this->decisionAlgo->backup(this->open, this->tlas, start);

            iterationCounter++;
        }

		return res;
    }

    void dumpClosedList(ofstream& out) {
        cout << "dump size: " << allGeneratedStates.size() << endl;
        for (typename unordered_map<State, DumpNode, Hash>::iterator it =
                        allGeneratedStates.begin();
                it != allGeneratedStates.end();
                it++) {
            out << it->first;
            out << it->second.h << " " << it->second.d << " " << it->first.key()
                << endl;
        }
    }

private:
    static void recordGenerated(Node* node) {
            if (allGeneratedStates.find(node->getState()) == allGeneratedStates.end()) {
                allGeneratedStates.emplace(
                        std::make_pair(node->getState(), DumpNode(node)));
            }
    }

    static bool duplicateDetection_recordGenerated(Node* node,
            unordered_map<State, Node*, Hash>& closed,
            PriorityQueue<Node*>& open,
            vector<TopLevelAction>& tlaList) {
        recordGenerated(node);
        // Check if this state exists
        typename unordered_map<State, Node*, Hash>::iterator it =
                closed.find(node->getState());

        if (it != closed.end()) {
            // This state has been generated before, check if its node is on
            // OPEN
            if (it->second->onOpen()) {
                // This node is on OPEN, keep the better g-value
                if (node->getGValue() < it->second->getGValue()) {
                    tlaList[it->second->getOwningTLA()].open.remove(it->second);
                    it->second->setGValue(node->getGValue());
                    it->second->setParent(node->getParent());
                    it->second->setHValue(node->getHValue());
                    it->second->setDValue(node->getDValue());
                    it->second->setDErrValue(node->getDErrValue());
                    it->second->setEpsilonH(node->getEpsilonH());
                    it->second->setEpsilonD(node->getEpsilonD());
                    it->second->setState(node->getState());
                    it->second->setOwningTLA(node->getOwningTLA());
                    tlaList[node->getOwningTLA()].open.push(it->second);
                }
            } else {
                // This node is on CLOSED, compare the f-values. If this new
                // f-value is better, reset g, h, and d.
                // Then reopen the node.
                if (node->getFValue() < it->second->getFValue()) {
                    it->second->setGValue(node->getGValue());
                    it->second->setParent(node->getParent());
                    it->second->setHValue(node->getHValue());
                    it->second->setDValue(node->getDValue());
                    it->second->setDErrValue(node->getDErrValue());
                    it->second->setEpsilonH(node->getEpsilonH());
                    it->second->setEpsilonD(node->getEpsilonD());
                    it->second->setState(node->getState());
                    it->second->setOwningTLA(node->getOwningTLA());
                    tlaList[node->getOwningTLA()].open.push(it->second);
                    it->second->reOpen();
                    open.push(it->second);
                }
            }

            return true;
        }

        return false;
	}

	static unordered_map<State, DumpNode, Hash> allGeneratedStates;
};

template<typename T>
unordered_map<typename LssLRTAStarSearch<T>::State, typename LssLRTAStarSearch<T>::DumpNode, typename LssLRTAStarSearch<T>::Hash> LssLRTAStarSearch<T>::allGeneratedStates;
