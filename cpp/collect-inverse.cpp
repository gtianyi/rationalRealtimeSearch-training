#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <random>
#include "domain/SlidingTilePuzzle.h"
#include "utility/PriorityQueue.h"
#include <boost/functional/hash.hpp>


using namespace std;


template<typename D>
struct hash_func  
{  
  size_t operator()(const shared_ptr<typename D::Node> n) const  
  {
    std::size_t hv=0;
    boost::hash_combine(hv,boost::hash<unsigned long long int>()(n->state.key()));
    return hv;  
  }  
};

//compare if two states are the same 
template<typename D>
struct cmp_func 
{  
  bool operator()(const shared_ptr<typename D::Node> n1, const shared_ptr<typename D::Node> n2) const  
  {
      return n1->state.key() == n2->state.key();
  }  
};

template<class Domain>
class Collection{
public:
    typedef typename Domain::State State;
    typedef typename Domain::Cost Cost;

    struct Node {
        Cost h;
        double d;
        State state;
        int hGroup;
        int frequencyCounter;

        Node(Cost _h, double _d, State _s)
                : h(_h), d(_d), state(_s), frequencyCounter(1) {
            hGroup = (int)floor(h / histInterval);
        }

        static bool compareNodesFeq(const shared_ptr<Node> a,
                const shared_ptr<Node> b) {
            return a->frequencyCounter < b->frequencyCounter;
        };
    };

    struct Bucket {
        Bucket()
                : minNonEmptyBucket(1) {}

        std::unordered_map<shared_ptr<Node>,int,
                hash_func<Collection<Domain>>,
                cmp_func<Collection<Domain>>>
                freqBuckListIndexMap;

		vector<shared_ptr<Node>> freqBuckList[1000];
		int minNonEmptyBucket;
		int size;
		

        void insert(shared_ptr<Node> n) {
			const auto& mapit = freqBuckListIndexMap.find(n);

            if (mapit != freqBuckListIndexMap.end()) {
                auto nodeInSide = mapit->first;
                auto& bucket = freqBuckList[nodeInSide->frequencyCounter];

                auto buckSize = bucket.size();
                if (buckSize != 1) {
                    bucket[mapit->second] = bucket[buckSize - 1];
                }

                bucket.pop_back();

                auto oldfc = nodeInSide->frequencyCounter;

                nodeInSide->frequencyCounter++;
                auto newBucket = min(nodeInSide->frequencyCounter, 999);

				freqBuckListIndexMap[nodeInSide]=freqBuckList[newBucket].size();

                freqBuckList[newBucket].push_back(nodeInSide);

                if (oldfc == minNonEmptyBucket)
                    fixMinimum(oldfc);
            } else {
                insert2bk(n);
            }
        }

    private:
        void fixMinimum(int i) {
            for (; i < 1000; i++) {
                if (!freqBuckList[i].empty()) {
                    minNonEmptyBucket = i;
                    return;
                }
            }
        }

        void insert2bk(shared_ptr<Node> n) {
            if (size > 1000000) {
                auto& bucket = freqBuckList[minNonEmptyBucket];
				auto worst = bucket[bucket.size()-1];
				freqBuckListIndexMap.erase(worst);
                bucket.pop_back();
                size--;
            }

            freqBuckListIndexMap[n] = freqBuckList[1].size();

            freqBuckList[1].push_back(n);

            size++;

            minNonEmptyBucket = 1;
        }
    };

    void parsingDumpFile(ifstream& f) {
        fileCount++;
        string line;

        getline(f, line);

        cout << "file #: " << fileCount << " ";
        cout << "file info: " << line << endl;

		string prevline;
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
                    if (tile >= 16) {
                        cout << "wrong tile " << tile <<" "<< r << " " << c << endl;
                        cout << "wrong line " << line << endl;
                        cout << "wrong prev line " << prevline << endl;
                    }
                    assert(tile < 16);
                    assert(tile >= 0);
                }
            }
            getline(f, line);
            stringstream ss2(line);

            prevline = line;

            Cost h;
            double d;
            string key;
            ss2 >> h;
            h = h / 15.0; // the h dumped out was weighted h, the weight was 15
            ss2 >> d;
            ss2 >> key;

            State s(board, 's');
            shared_ptr<Node> n = make_shared<Node>(h, d, s);

            auto& bucket = hCollection[n->hGroup];

			if (n->hGroup >= htableSize){
				std::cout << "high h: " << h << "\n";
				continue;
			}

            assert(n->hGroup < htableSize);

            bucket.insert(n);

            /*if (bucket.size() > sampleCount)*/
			//{
                //// obtain a random number from hardware
                //std::random_device rd;

                //// seed the generator
                //std::mt19937 eng(rd());

                //// define the range
                //std::uniform_int_distribution<> distr(0, bucket.size() - 1);

                //int randPos = distr(eng);

                //bucket.erase(std::next(bucket.begin(), randPos));
            /*}*/
        }
    };

    void dumpSampleSet(string tileType) {
        int id = 0;
        int hcount = 0;

        string fileFrequencyRecord =
                "../results/SlidingTilePuzzle/sampleProblem/" + tileType + "/" +
                "0FrequencyCounter.txt";

        ofstream counterFile(fileFrequencyRecord);

        for (int i = 0; i < htableSize; i++) {
            auto& bucket = hCollection[i];

            if (bucket.size == 0)
                continue;

            hcount++;

			int bucketDump = 0;
            for (int freqC = 999; freqC >= 0; freqC--) {
                if (bucket.freqBuckList[freqC].empty())
                    continue;

                if (bucketDump > 200)
                    break;

                for (auto n : bucket.freqBuckList[freqC]) {
                    if (bucketDump > 200)
                        break;

					bucketDump++;

                    id++;
                    string fileName = "../results/SlidingTilePuzzle/"
                                      "sampleProblem/" +
                            tileType + "/" + to_string(id) + ".st";

                    ofstream f(fileName);

                    // std::cout << (*it)->state << "\n";

                    n->state.dumpToProblemFile(f);
                    f.close();

                    counterFile << to_string(id) << " " << n->frequencyCounter
                                << "\n";
                }
            }
		}
         
        counterFile.close();

        cout << "h count " << hcount << endl;
        cout << "dump count " << id << endl;
    }

	Collection() : fileCount(0){};

private:
    static constexpr double histInterval = 0.1;
    static constexpr double histMax = 20.0;
    static constexpr int htableSize = (int)histMax / histInterval;

    Bucket hCollection[htableSize];

    int fileCount;
};

int main(int argc, char** argv) {
    if (argc != 6) {
        cout << "Wrong number of arguments: ./collect-inverse <weight> <first "
                "instance> <last instance> <tile type> <sample count>"
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

    collection.dumpSampleSet(tileType);
};

