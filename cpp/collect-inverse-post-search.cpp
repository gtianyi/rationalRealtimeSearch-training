#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <random>
#include "domain/InverseTilePuzzle.h"
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

string double2string(double c, int precision) {
        stringstream stream;
        stream << fixed << setprecision(precision) << c;
        return stream.str();
}

constexpr int hPrecision = 1;

template<class Domain, class hValueType>
class PostSearchCollection{
public:
    typedef typename Domain::State State;
    typedef typename Domain::Cost Cost;

    using HStateMap =
            std::unordered_map<std::string, std::vector<State>>;

    struct Hist {
        struct Bin {
            Bin(hValueType hs, int count) : value(hs), count(count) {}

            void setValue(hValueType v) {
                value = v;
                valueString = double2string(v, hPrecision);
            }

            hValueType getValue() const { return value; }
            string getValueString() const { return valueString; }

            void setCount(int c) { count = c; }
            int getCount() const { return count; }
            void addCount(int c) { count += c; }

            void print(ofstream& f) const { f << valueString << " " << count << " "; }

        private:
            hValueType value;
            string valueString;
            int count;

        };

        vector<Bin> bins;
        double mean;
        int totalCount;

        void push(Bin&& bin) {
            mean = (mean * totalCount + bin.getValue() * bin.getCount()) /
                    (double)(totalCount + bin.getCount());
            totalCount += bin.getCount();
            bins.push_back(std::move(bin));
        }

        void updateBin(int binIndex, int addcount) {
            auto& bin = bins[binIndex];
            mean = (mean * totalCount + bin.getValue() * addcount) /
                    (double)(totalCount + addcount);

            bin.addCount(addcount);
            totalCount += addcount;
        }

        Hist shift(Cost c) {
            Hist retHist;

            retHist.mean = mean + c;
            retHist.totalCount = totalCount;

            for (auto bin : bins) {
                Bin newBin(bin.getValue() + c,bin.getCount());
                retHist.bins.push_back(newBin);
            }

            return retHist;
        }

        void print(ofstream& f) const {
            f << totalCount << " ";
            for (const auto& bin : bins) {
                bin.print(f);
            }
            f << "\n";
        }
    };

    typedef typename Hist::Bin Bin;

    State getStateByInstanceName(const string& instance) {
        string fileName = "../results/SlidingTilePuzzle/sampleProblem/" + instance;
        std::ifstream f(fileName);
		Domain d(f);
		return d.getStartState();
	}

    void parsingSamples(ifstream& f) {
        string line, h, instanceName;

        while (getline(f, line)) {
            stringstream ss(line);

            ss >> h;
            int valueCount;
            ss >> valueCount;

            vector<State> statesList;

            while (!ss.eof()) {
                ss >> instanceName;
				statesList.push_back(getStateByInstanceName(instanceName));
            }

            assert(valueCount == statesList.size());

            hStatesCollection[h] = statesList;
        }

        f.close();
    }

    void parseHistData(ifstream& f) {
        string line;

		string h;
		hValueType hs;
		int valueCount, hsCount;

        while (getline(f, line)) {
            stringstream ss(line);

            ss >> h;
            ss >> valueCount;

			Hist hist;
            while (!ss.eof()) {
                ss >> hs;
                ss >> hsCount;
				hist.push(Bin(hs, hsCount));
            }

            assert(hist.bins.size() == valueCount);
			originalhHist[h]=hist;
        }

		f.close();
    }

    Hist lookahead(const State& s) {
        Cost backuphhat = std::numeric_limits<double>::infinity();

        vector<State> children = domain.successors(s);
        Hist bestShiftedHist;

        for (auto& c : children) {
            Cost h = domain.heuristic(c);
            auto hString = double2string(h, hPrecision);
            Cost hhat = originalhHist[hString].mean;
            Cost edgeCost = domain.getEdgeCost(c);
            hhat += edgeCost;
            if (hhat < backuphhat) {
                backuphhat = hhat;
                bestShiftedHist = originalhHist[hString].shift(edgeCost);
            }
        }

        return bestShiftedHist;
    }

    Hist mergeHists(const vector<Hist>& hists) {
        Hist newHist;
        unordered_map<string, int> newHistKey;

        for (auto hist : hists) {
            for (auto bin : hist.bins) {
                if (newHistKey.find(bin.getValueString()) != newHistKey.end()) {
                    int binIndex = newHistKey[bin.getValueString()];
                    newHist.updateBin(binIndex, bin.getCount());
                }
				else{
                    newHist.push(std::move(bin));
                    newHistKey[bin.getValueString()] = newHist.bins.size() - 1;
                }
            }
        }
	}

    void computePostSearchHist(ifstream& f) {
        parseHistData(f);

        for (auto it = hStatesCollection.begin(); it != hStatesCollection.end();
                it++) {
            std::vector<Hist> histListAfterBackup;

            for (auto s : it->second) {
                auto hist = lookahead(s);
				histListAfterBackup.push_back(hist);
            }

            auto newHist = mergeHists(histListAfterBackup);

            postSearchhHist[it->first] = newHist;
        }
    }

    void dumpPostSearchHist(ofstream& f) {
        for (auto it = postSearchhHist.begin(); it != postSearchhHist.end();
                it++) {
            auto& h = it->first;
            auto& hist = it->second;

            f << h << " ";
            hist.print(f);
        }

		f.close();
	}
    
private:
    Domain domain;
    HStateMap hStatesCollection;
    std::unordered_map<std::string, Hist> originalhHist;
    std::unordered_map<std::string, Hist> postSearchhHist;
};

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Wrong number of arguments: ./collect-post-search <samples file> <distribution file>"
             << endl;
        exit(1);
    }

    string sampleFile = "../results/SlidingTilePuzzle/sampleProblem/" +
            std::string(argv[1]);
    string distributionFile = "../results/SlidingTilePuzzle/sampleProblem/" +
            std::string(argv[2]);
    string postSearchFile =
            "../results/SlidingTilePuzzle/sampleProblem/postSearch" +
            std::string(argv[2]);

    std::ifstream f_sample(sampleFile);
    std::ifstream f_distribution(distributionFile);
    std::ofstream f_out(postSearchFile);

    PostSearchCollection<InverseTilePuzzle,float> post_search_collection;

    // read each line get all 200-ish samples
	post_search_collection.parsingSamples(f_sample);

    // for each sample problem, expand it find its succssors and their h-values
    // query the h* distributions by the h-values
    // do nancy backup (eg, get the one with min f-hat), to get the post-search
    // belief
	// for all 200-ish post-search beliefs, merge them by doing numerical
    // intergation
    // now we have the mapping that map h to post-search h*
	post_search_collection.computePostSearchHist(f_distribution);

	post_search_collection.dumpPostSearchHist(f_out);
}
