#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <random>
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
        int frequencyCounter;

        Node() = delete;
        Node(int _h, int _d, State _s)
                : h(_h), d(_d), state(_s), frequencyCounter(1) {}
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
            } else {
                nodeCollection[key]->frequencyCounter++;
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

    void sampleStatesForH(int sampleCount) {
			
        cout << "h count " << hCollection.size() << endl;
        for (typename unordered_map<int,
                     std::vector<shared_ptr<Node>>>::iterator it =
                        hCollection.begin();
                it != hCollection.end();
                it++) {
            if (it->second.size() <= sampleCount) {
                sampleSet.insert(
                        sampleSet.end(), it->second.begin(), it->second.end());
            } else {
                // obtain a random number from hardware
                std::random_device rd;

                // seed the generator
                std::mt19937 eng(rd());

                // define the range
                std::uniform_int_distribution<> distr(
                        0, it->second.size() - 1); 

				//sample from range
                unordered_set<int> sampleIDSet;

                while (sampleIDSet.size() < sampleCount) {
                    sampleIDSet.insert(distr(eng)); // generate numbers
                }

                for (typename unordered_set<int>::iterator idit =
                                sampleIDSet.begin();
                        idit != sampleIDSet.end();
                        idit++) {
                    sampleSet.push_back(it->second[*idit]);
                }
            }
        }
    }

    void dumpSampleSet(string tileType) {
        int id = 0;

        string fileFrequencyRecord =
                "../results/SlidingTilePuzzle/sampleProblem/" + tileType + "/" +
                "0FrequencyCounter.txt";

        ofstream counterFile(fileFrequencyRecord);

        for (auto n : sampleSet) {
            id++;
            string fileName = "../results/SlidingTilePuzzle/sampleProblem/" +
                    tileType + "/" + to_string(id) + ".st";

            ofstream f(fileName);

            n->state.dumpToProblemFile(f);
            f.close();

            counterFile << to_string(id) << " " << n->frequencyCounter << "\n";
        }
        counterFile.close();
        cout << "dump count " << id << endl;
    }

    Collection() : fileCount(0){};

private:
    unordered_map<unsigned long long, shared_ptr<Node>> nodeCollection;
    unordered_map<int, std::vector<shared_ptr<Node>>> hCollection;
	vector<shared_ptr<Node>> sampleSet;

    int fileCount;

};

int main(int argc, char** argv) {
    if (argc != 6) {
        cout << "Wrong number of arguments: ./collect <weight> <first instance> <last instance> <tile type> <sample count>"
             << endl;
        exit(1);
    }

    string weight = argv[1];

    int firstNum = stoi(argv[2]);

    int lastNum = stoi(argv[3]);

	string tileType = argv[4];

    int sampleCount = stoi(argv[5]);


	Collection<SlidingTilePuzzle> collection;

    for (int i = firstNum; i <= lastNum; i++) {
        string fileName = "../results/SlidingTilePuzzle/distributionTest/" +
                tileType + "/W" + weight + "-" + std::to_string(i) + ".txt";

        std::ifstream f(fileName);

        collection.parsingDumpFile(f);

        f.close();
    }

	collection.arrangeCollectionByH();

	collection.sampleStatesForH(sampleCount);

	collection.dumpSampleSet(tileType);
}
