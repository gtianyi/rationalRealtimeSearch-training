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
#include "domain/PancakePuzzle.h"
#include "utility/cxxopts/include/cxxopts.hpp"


using namespace std;

class CollectionBase {
public:
    virtual void parsingDumpFile(ifstream& f, string& domainSize) = 0;
    virtual void arrangeCollectionByH() = 0;
    virtual void sampleStatesForH(int sampleCount) = 0;
    virtual void dumpSampleSet(string tileType, string alg) = 0;
};

template <class Domain>
class Collection : virtual public CollectionBase {
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

    void arrangeCollectionByH() {
        cout << "observed states " << nodeCollection.size() << endl;
        cout << "arrange collection by h " << endl;
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
                     vector<shared_ptr<Node>>>::iterator it =
                        hCollection.begin();
                it != hCollection.end();
                it++) {
            if (it->second.size() <= sampleCount) {
                sampleSet.insert(
                        sampleSet.end(), it->second.begin(), it->second.end());
            } else {
                sort(it->second.begin(), it->second.end(), NodeCompare());
                sampleSet.insert(sampleSet.end(),
                        it->second.begin(),
                        it->second.begin() + sampleCount);
            }
        }
    }

    void dumpSampleSet(string dumpPath, string alg) {
        int id = 0;

        string mkdirCMD = "mkdir -p " + dumpPath;
        system(mkdirCMD.c_str());

        string fileFrequencyRecord = dumpPath + "/" + "0FrequencyCounter.txt";

        ofstream counterFile(fileFrequencyRecord);

        for (auto n : sampleSet) {
            id++;
            string fileName = dumpPath + "/" + to_string(id) + ".st";

            ofstream f(fileName);

            n->state.dumpToProblemFile(f);
            f.close();

            counterFile << to_string(id) << " " << n->frequencyCounter << "\n";
        }
        counterFile.close();
        cout << "dump count " << id << endl;
    }

    Collection() : fileCount(0){};

protected:
    unordered_map<unsigned long long, shared_ptr<Node>> nodeCollection;
    unordered_map<int, vector<shared_ptr<Node>>> hCollection;
	vector<shared_ptr<Node>> sampleSet;

    int fileCount;
};

template <class Domain>
class CollectionTile : public Collection<Domain> {
    typedef typename Domain::State State;
    typedef typename Collection<Domain>::Node Node;

    void parsingDumpFile(ifstream& f, string& domainSize) {
        this->fileCount++;
        string line;

        getline(f, line);

        cout << "file #: " << this->fileCount << " ";
        cout << "file info: " << line << endl;

        while (!f.eof()) {
            vector<int> rows(4, 0);
            vector<vector<int>> board(4, rows);
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

            if (this->nodeCollection.find(key) == this->nodeCollection.end()) {
                State s(board, 's');
                shared_ptr<Node> n = make_shared<Node>(h, d, s);
                this->nodeCollection[key] = n;
            } else {
                this->nodeCollection[key]->frequencyCounter++;
            }
        }
    };
};

template <class Domain>
class CollectionPancake : public Collection<Domain> {
    typedef typename Domain::State State;
    typedef typename Collection<Domain>::Node Node;

    void parsingDumpFile(ifstream& f, string& domainSize) {
        int size = stoi(domainSize);
        this->fileCount++;
        string line;

        getline(f, line);

        cout << "file #: " << this->fileCount << " ";
        cout << "file info: " << line << endl;

        while (!f.eof()) {
            vector<unsigned char> pcake;
            getline(f, line);
            stringstream ss(line);
            for (int p = 0; p < size; p++) {
				 unsigned char pan;
				 ss >> pan;
				 pcake.push_back(pan);
            }

			int h, d;
			unsigned long long key;
			ss >> h;
			ss >> d;
			ss >> key;

			if (this->nodeCollection.find(key) == this->nodeCollection.end()) {
				State s(pcake, 0);
				shared_ptr<Node> n = make_shared<Node>(h, d, s);
				this->nodeCollection[key] = n;
			} else {
				this->nodeCollection[key]->frequencyCounter++;
			}
		}
	};
};


int main(int argc, char** argv) {
    cxxopts::Options options("./collect", "This is program is for collect "
                                          "obesrved states given the output "
                                          "files of an suboptimal serach");

    options.add_options()

		("d,domain", "domain type: randomtree, tile, pancake", 
		 cxxopts::value<string>()->default_value("pancake"))

        ("s,subdomain", "puzzle type: uniform, inverse, heavy, sqrt; pancake type: regular, heavy", 
		 cxxopts::value<string>()->default_value("regular"))

        ("a,alg", "suboptimal algorithm: wastar, lsslrtastar", 
		 cxxopts::value<string>()->default_value("wastar"))

        ("p,par", "weight for weighted A*, lookahead for lsslrta*", 
		 cxxopts::value<string>()->default_value("2"))

		("z,size", "domain size, 16 for pancake, 4x4 for tile", 
		 cxxopts::value<string>()->default_value("16"))

		("c,count", "sample count", 
		 cxxopts::value<int>()->default_value("200"))

		("f,first", "first instance", cxxopts::value<int>()->default_value("1"))

		("l,last", "last instance", cxxopts::value<int>()->default_value("100"))

        ("h,help", "Print usage")
    ;

    auto args = options.parse(argc, argv);

    if (args.count("help")) {
        cout << options.help() << endl;
        exit(0);
    }

    string domain = args["domain"].as<string>();
    string subdomain = args["subdomain"].as<string>();
    string alg = args["alg"].as<string>();
    string algPara = args["par"].as<string>();
    string size = args["size"].as<string>();
    int firstNum = args["first"].as<int>();
    int lastNum = args["last"].as<int>();
    int sampleCount = args["count"].as<int>();

    shared_ptr<CollectionBase> collectionPtr;
	string inPath;
	string outPath;


    if (domain == "tile") {

        collectionPtr = make_shared<CollectionTile<SlidingTilePuzzle>>();

        inPath = "../results/SlidingTilePuzzle/distributionTest/" +
                // tileType + "/" + alg + "/Para" + algPara + "-" +
                subdomain + "/" + alg + "/W" + algPara + "-";

		outPath = "../results/SlidingTilePuzzle/sampleProblem/" +
                subdomain + "/" + alg;

    } else if (domain == "pancake") {

        collectionPtr = make_shared<CollectionPancake<PancakePuzzle>>();

        inPath = "../results/" + domain + "/distributionTest/" + subdomain +
                "/" + alg + "/Para" + algPara + "-" + size + "-";

        outPath = "../results/" + domain + "/sampleProblem/" + subdomain + "/" +
                alg + "/Para" + algPara + "/" + size;
    }

    for (int i = firstNum; i <= lastNum; i++) {

        string fileName = inPath + to_string(i) + ".txt";

        ifstream f(fileName);

        collectionPtr->parsingDumpFile(f, size);

        f.close();

    }

	collectionPtr->arrangeCollectionByH();

	collectionPtr->sampleStatesForH(sampleCount);

	collectionPtr->dumpSampleSet(outPath, alg);

}
