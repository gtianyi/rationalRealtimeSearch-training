#pragma once
#include <vector>
#include <set>
#include <functional>
#include <unordered_map>
#include "SearchBase.h"

using namespace std;

template <class Domain>
class WAStarSearch : public Search {
public:
    typedef typename Domain::State State;
    typedef typename Domain::Cost Cost;
    typedef typename Domain::HashState Hash;

    struct Node {
        Cost g;
        Cost h;
        Cost unWeightedH;
		Cost d;
        Node* parent;
        State stateRep;
		bool open;

    public:
        Cost getGValue() const { return g; }
        Cost getHValue() const { return h; }
        Cost getFValue() const { return g + h; }
        Cost getDValue() const { return d; }
        State getState() const { return stateRep; }
        Node* getParent() const { return parent; }

        Cost getUnweightedHValue() const { return unWeightedH; }

        void setHValue(Cost val) { h = val; }
        void setGValue(Cost val) { g = val; }
        void setDValue(Cost val) { d = val; }
        void setState(State s) { stateRep = s; }
        void setParent(Node* p) { parent = p; }

        Node(Cost g, Cost h, Cost unWeightedH, State state, Node* parent)
                : g(g),
                  h(h),
                  unWeightedH(unWeightedH),
                  stateRep(state),
                  parent(parent),
                  open(true) {}

        friend std::ostream& operator<<(std::ostream& stream,
                const Node& node) {
            stream << node.getState() << endl;
            stream << "f: " << node.getFValue() << endl;
            stream << "g: " << node.getGValue() << endl;
            stream << "h: " << node.getHValue() << endl;
            stream << "action generated by: " << node.getState().getLabel()
                   << endl;
            stream << "-----------------------------------------------" << endl;
            stream << endl;
            return stream;
        }

        bool onOpen() { return open; }
        void close() { open = false; }

        static bool compareNodesF(const Node* n1, const Node* n2) {
            // Tie break on g-value
            if (n1->getFValue() == n2->getFValue()) {
                return n1->getGValue() > n2->getGValue();
            }
            return n1->getFValue() < n2->getFValue();
        }

    };

    WAStarSearch(Domain& domain,
            float weight)
            : domain(domain),
              weight(weight) {
        wastar =
                new WAStar<Domain, Node>(domain, weight, "f");
         }

    ~WAStarSearch() { clean(); }

    SearchResultContainer doSearch() {
        SearchResultContainer res;

        auto initUnweightedH = domain.heuristic(domain.getStartState());
        auto inith = initUnweightedH * weight;

        // Get the start node
        Node* cur = new Node(
                0, inith, initUnweightedH, domain.getStartState(), NULL);

        open.push(cur);
        res.initialH = initUnweightedH;

        // Expand some nodes
        double solutionCost =
                wastar->expand(open, closed, duplicateDetection, res);

        calculateCost(solutionCost, res);

        return res;
    }

    void dumpClosedList(ofstream& out) {
        for (typename unordered_map<State, Node*, Hash>::iterator it =
                        closed.begin();
                it != closed.end();
                it++) {
            out << it->first;
            out << it->second->getUnweightedHValue() << " " << it->second->getDValue()
                << " " << it->first.key() << endl;

        }
    }

private:
    static bool duplicateDetection(Node* node,
            unordered_map<State, Node*, Hash>& closed,
            PriorityQueue<Node*>& open) {
        // Check if this state exists
        typename unordered_map<State, Node*, Hash>::iterator it =
                closed.find(node->getState());

        if (it != closed.end()) {
            // This state has been generated before, check if its node is on
            // OPEN
            if (it->second->onOpen()) {
                // This node is on OPEN, keep the better g-value
                if (node->getGValue() < it->second->getGValue()) {
                    it->second->setGValue(node->getGValue());
                    it->second->setParent(node->getParent());
                    it->second->setHValue(node->getHValue());
                    it->second->setDValue(node->getDValue());
                    it->second->setState(node->getState());
                }
            }

            return true;
        }

        return false;
    }

    void clean() {
        // Empty OPEN and CLOSED
        open.clear();

        // delete all of the nodes from the last expansion phase
        for (typename unordered_map<State, Node*, Hash>::iterator it =
                        closed.begin();
                it != closed.end();
                it++)
            delete it->second;

        closed.clear();

        delete wastar;
    }

    void calculateCost(double solutionCost, SearchResultContainer& res) {
        res.solutionFound = true;
        res.solutionCost = solutionCost;
    }

protected:
    Domain& domain;
    WAStar<Domain, Node>* wastar;
    PriorityQueue<Node*> open;
    unordered_map<State, Node*, Hash> closed;

    float weight;
};
