#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <random>
#include <climits>
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

    struct NodeCompare {
        bool operator()(const shared_ptr<Node> l, const shared_ptr<Node> r) {
            return l->frequencyCounter > r->frequencyCounter;
        }
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
                std::sort(it->second.begin(), it->second.end(), NodeCompare());
                sampleSet.insert(sampleSet.end(),
                        it->second.begin(),
                        it->second.begin() + sampleCount);
            }
        }
    }

    void dumpSampleSet(string tileType, string alg) {
        int id = 0;

        string mkdirCMD =
                "mkdir -p ../results/SlidingTilePuzzle/sampleProblem/" +
                tileType + "/" + alg;
        std::system(mkdirCMD.c_str());

        string fileFrequencyRecord =
                "../results/SlidingTilePuzzle/sampleProblem/" + tileType + "/" +
                alg + "/" + "0FrequencyCounter.txt";

        ofstream counterFile(fileFrequencyRecord);

        for (auto n : sampleSet) {
            id++;
            string fileName = "../results/SlidingTilePuzzle/sampleProblem/" +
                    tileType + "/" + alg + "/" + to_string(id) + ".st";

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
    if (argc != 7) {
        cout << "Wrong number of arguments: ./collect <alg> <algPara> <first instance> <last instance> <tile type> <sample count>"
             << endl;
        exit(1);
    }

	string alg = argv[1];

    string algPara = argv[2];

    int firstNum = stoi(argv[3]);

    int lastNum = stoi(argv[4]);

	string tileType = argv[5];

    int sampleCount = stoi(argv[6]);

	sampleCount = INT_MAX;

    Collection<SlidingTilePuzzle> collection;

    for (int i = firstNum; i <= lastNum; i++) {
        string fileName = "../results/SlidingTilePuzzle/distributionTest/" +
                //tileType + "/" + alg + "/Para" + algPara + "-" +
                tileType + "/" + alg + "/W" + algPara + "-" +
                std::to_string(i) + ".txt";

        std::ifstream f(fileName);

        collection.parsingDumpFile(f);

        f.close();
    }

	collection.arrangeCollectionByH();

	collection.sampleStatesForH(sampleCount);

	collection.dumpSampleSet(tileType, alg);
}
