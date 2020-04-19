#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

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
