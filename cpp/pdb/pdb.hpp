/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: pdb.cpp
 * description: patthern database for fifteen puzzle
 * date: March-25-2019
 *
 * update By: Tianyi Gu
 * update date: Aug-25-2019
 * update : enable heavy tile and inverse tile
 */

#include "partialTiles.hpp"
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>

using namespace std;

template<class TileType>
class PDB {
    struct Node {
        float g;
        int pop;
        typename TileType::PackedState packed;
		HashEntry<Node> hentry;

        const typename TileType::PackedState& key() { return packed; }

        HashEntry<Node>& hashentry() { return hentry; }

        Node(typename TileType::State& s) {
            g = 0;
            TileType dom;
            dom.pack(packed, s);
            pop = -1;

			dom.printState(s);
			typename TileType::State state;
			dom.unpack(state,packed);
			dom.printState(state);
        }

        Node(typename TileType::State& s, Node* p, float c,int _pop) {
            g = p->g + c;
            TileType dom;
			dom.pack(packed, s);
			pop = _pop;
        }
    };

    void generatePartialPDB(int patternSize, string filename) {
        TileType dom(patternSize);

        string fname = "../results/SlidingTilePuzzle/pdb/" + filename + "-" +
                dom.getType() + ".txt";

        std::ofstream f(fname);

        HashTable<typename TileType::PackedState, Node> closed(512927357);

        queue<Node*> open;

        typename TileType::State initState = dom.initial();

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

            typename TileType::State state;
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
