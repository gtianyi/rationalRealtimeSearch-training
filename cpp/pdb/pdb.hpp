/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: pdb.cpp
 * description: patthern database for fifteen puzzle
 * date: March-25-2019
 */

#include "partialTiles.hpp"
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>

using namespace std;

class PDB {
    struct Node {
        float g;
        int pop;
        typename PartialTiles::PackedState packed;
		HashEntry<Node> hentry;

        const typename PartialTiles::PackedState& key() { return packed; }

        HashEntry<Node>& hashentry() { return hentry; }

        Node(PartialTiles::State& s) {
            g = 0;
            PartialTiles dom;
            dom.pack(packed, s);
            pop = -1;

			dom.printState(s);
			PartialTiles::State state;
			dom.unpack(state,packed);
			dom.printState(state);
			
        }

        Node(PartialTiles::State& s, Node* p, float c,int _pop) {
            g = p->g + c;
            PartialTiles dom;
			dom.pack(packed, s);
			pop = _pop;
        }
    };

    void generatePartialPDB(int patternSize, string filename) {
        string fname = "../results/SlidingTilePuzzle/pdb/" + filename + ".txt";

		std::ofstream f(fname);
		
        PartialTiles dom(patternSize);

        HashTable<typename PartialTiles::PackedState, Node> closed(512927357);

        queue<Node*> open;

        PartialTiles::State initState = dom.initial();

        Node* initNode = new Node(initState);

        open.push(initNode);

        // do BFS backward from goal;
        while (!open.empty()) {
            Node* n = open.front();
            open.pop();

            if (closed.find(n->packed)) {
                continue;
				delete n;
            }

            PartialTiles::State state;
            dom.unpack(state, n->packed);

            closed.add(n);
            f << n->packed.word << " " << n->g << endl;

			//dom.printState(state);
            for (int i = 0; i < dom.nops(state); i++) {
                int op = dom.nthop(state, i);
                if (op == n->pop)
                    continue;
				//dom.printState(state);
                Edge<Tiles> e = dom.apply(state, op);
                Node* child = new Node(state, n, e.cost, e.pop);
				//dom.printState(state);
                open.push(child);
                dom.undo(state, e);
            }
        }

        cout << filename << " " << closed.getSize() << endl;
        f.close();
    }

public:
    void generateDisjointPDB(string patternSize) {
        generatePartialPDB(stoi(patternSize), patternSize);
    }
};
