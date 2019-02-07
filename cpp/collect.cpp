#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>
#include "domain/SlidingTilePuzzle.h"


using namespace std;

template<class Domain>
class Collection{
public:
    typedef typename Domain::State State;
    typedef typename Domain::Cost Cost;

    struct Node {
        int h;
        int d;
        State state;

        Node(int _h, int _d, State _s) : h(_h), d(_d), state(_s) {}
    };

    void parsingDumpFile(ifstream& f) {
        fileCount++;
        string line;

        getline(f, line);

        cout << "file #: " << fileCount << " ";
        cout << "file info: " << line << endl;

        while (!f.eof()) {
            std::vector<int> rows(4, 0);
            std::vector<std::vector<int>> board(4, rows);
            for (int r = 0; r < 4; r++) {
                getline(f, line);
                stringstream ss(line);
                for (int c = 0; c < 4; c++) {
                    int tile;
                    ss >> tile;
					board[r][c] = tile;
                }
            }
            getline(f, line);
            stringstream ss2(line);

            int h, d;
            unsigned long long key;
            ss2 >> h;
            ss2 >> d;
            ss2 >> key;

            if (nodeCollection.find(key) == nodeCollection.end()) {
                State s(board, 's');
                shared_ptr<Node> n = make_shared<Node>(h, d, s);
                nodeCollection[key] = n;
            }
        }
    };

    void arrangeCollectionByH() {
        for (typename unordered_map<unsigned long long,
                     shared_ptr<Node>>::iterator it = nodeCollection.begin();
                it != nodeCollection.end();
                it++) {
            hCollection[it->second->h].push_back(it->second);
        }
    }

    void sampleStatesForH() {
        for (typename unordered_map<int,
                     std::vector<shared_ptr<Node>>>::iterator it =
                        hCollection.begin();
                it != hCollection.end();
                it++) {
            cout << "h " << it->first << " allN: " << it->second.size() << endl;
        }
	}

    Collection() : fileCount(0){};

private:
    unordered_map<unsigned long long, shared_ptr<Node>> nodeCollection;
    unordered_map<int, std::vector<shared_ptr<Node>>> hCollection;

    int fileCount;

};

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Wrong number of arguments: ./collect <weight> <first instance> <last instance>"
             << endl;
        exit(1);
    }

    string weight = argv[1];

    int firstNum = stoi(argv[2]);

    int lastNum = stoi(argv[3]);

	Collection<SlidingTilePuzzle> collection;

    for (int i = firstNum; i <= lastNum; i++) {
        string fileName = "../results/SlidingTilePuzzle/distributionTest/W" +
                weight + "-" + std::to_string(i) + ".txt";

		std::ifstream f(fileName);

		collection.parsingDumpFile(f);

		f.close();
    }

	collection.arrangeCollectionByH();
	collection.sampleStatesForH();
}
