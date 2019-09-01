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
#include "domain/HeavyTilePuzzle.h"
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
constexpr int hsPrecision = 1;
constexpr double hSearchDownStep = 1;

struct CollectionBase{
    virtual void parsingSamples(ifstream& f, const string& instanceDir)=0; 
    virtual void computePostSearchHist(ifstream& f)=0;
    virtual void dumpPostSearchHist(ofstream& f) const = 0;
};

template <class Domain>
class PostSearchCollection : public CollectionBase {
public:
  virtual void parsingSamples(ifstream& f,
            const string& instanceDir) override {
        string line, h, instanceName;

        while (getline(f, line)) {
            stringstream ss(line);

            ss >> h;
            int valueCount;
            ss >> valueCount;

            vector<State> statesList;

            while (ss >> instanceName) {
                RawState s = getStateByInstanceName(instanceName, instanceDir);
                Cost deltaH;
                ss >> deltaH;

                statesList.push_back(State(s,deltaH));
            }

            //assert(valueCount == statesList.size());

            hStatesCollection[h] = statesList;
        }

        f.close();
    }

    virtual void computePostSearchHist(ifstream& f) override {
        parseHistData(f);

        for (auto it = hStatesCollection.begin(); it != hStatesCollection.end();
                it++) {
            std::vector<Hist> histListAfterBackup;

            for (auto s : it->second) {
				auto rawHist = lookahead(s.rawS);
				histListAfterBackup.push_back(rawHist.shift(s.deltaH));
            }

            postSearchhHist[it->first] =
                    mergeHists(std::move(histListAfterBackup));
        }
    }

    virtual void dumpPostSearchHist(ofstream& f) const override{
        // sort by h before dump
        vector<string> keys;

        for (auto& it : postSearchhHist) {
            keys.push_back(it.first);
        }

		sort(keys.begin(),keys.end());

        for (auto& h:keys) {
            auto& hist = postSearchhHist.at(h);

            f << h << " ";
            hist.print(f);
        }

		f.close();
	}

private:

    typedef typename Domain::State RawState;
    typedef typename Domain::Cost Cost;

    struct State {
        RawState rawS;
        // h of missing - h of the state
		// eg: h = 3 have no data
		//     borow one instance from h = 5
		//     delta h = 3-5 = -2
        Cost deltaH; 
        State() = delete;
        State(RawState& s, Cost deltaH) : rawS(s), deltaH(deltaH){};
    };

    using HStateMap = std::unordered_map<std::string, std::vector<State>>;

    struct Hist {
        struct Bin {
            Bin(Cost hs, int count) : value(hs), count(count) {
                valueString = double2string(hs, hsPrecision);
            }

            void setValue(Cost v) {
                value = v;
                valueString = double2string(v, hsPrecision);
            }

            Cost getValue() const { return value; }
            string getValueString() const { return valueString; }

            void setCount(int c) { count = c; }
            int getCount() const { return count; }
            void addCount(int c) { count += c; }

            void print(ofstream& f) const {
                f << valueString << " " << count << " ";
            }

        private:
            Cost value;
            string valueString;
            int count;
        };

        Hist() : mean(0), totalCount(0) {}

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

        Hist shift(Cost c) const {
            Hist retHist;

            retHist.mean = mean + c;
            retHist.totalCount = totalCount;

            for (auto bin : bins) {
                Bin newBin(bin.getValue() + c, bin.getCount());
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

        vector<Bin> getBins() const { return bins; }

        double getMean() const { return mean; }

        int getTotalCount() const { return totalCount; }

    private:
        vector<Bin> bins;
        double mean;
        int totalCount;
    };

    typedef typename Hist::Bin Bin;

    RawState getStateByInstanceName(const string& instance,
            const string& dir) const {
        string fileName = "../results/SlidingTilePuzzle/sampleProblem/" + dir +
                "/" + instance;
        std::ifstream f(fileName);
        Domain d(f);
        return d.getStartState();
    }
  
    void parseHistData(ifstream& f) {
        string line;

		string hstring;
		Cost hs, hvalue;
		int valueCount, hsCount;

        while (getline(f, line)) {
            stringstream ss(line);

            ss >> hvalue;
			hstring = double2string(hvalue, hPrecision);
            ss >> valueCount;

			Hist hist;
            while (ss >> hs) {
                ss >> hsCount;
				hist.push(Bin(hs, hsCount));
            }

            //assert(hist.getTotalCount() == valueCount);
			originalhHist[hstring]=hist;
			originalhValues.push_back(hvalue);
        }

		std::sort(originalhValues.begin(),originalhValues.end());

		f.close();
    }

    Hist findorShiftFromClosestHist(const Cost h) const{
        auto hString = double2string(h, hPrecision);

        int steps = 0;

        if (originalhHist.find(hString) != originalhHist.end()) {
            return originalhHist.at(hString);
        } else {
            Cost closesth;

            if (h > originalhValues[originalhValues.size() - 1]) {
                closesth = originalhValues[originalhValues.size() - 1];
                hString = double2string(closesth, hPrecision);
            } else if (h < originalhValues[1]) {
                // 0 is goal, we want to return the smallest h value for
                // a non-goal state
                closesth = originalhValues[1];
                hString = double2string(closesth, hPrecision);
            } else {
                // if it is in the middle, we go down until reach one.
                auto curH = h;
                while (curH > originalhValues[1]) {
                    curH = curH - hSearchDownStep;
                    hString = double2string(curH, hPrecision);
                    steps++;
                    if (originalhHist.find(hString) != originalhHist.end()) {
                        break;
                    }
                }
            }
        }

        return originalhHist.at(hString).shift((Cost)steps * hSearchDownStep);
    }

    Hist lookahead(const RawState& s) const {
        Cost backuphhat = std::numeric_limits<double>::infinity();

        Domain d;

        auto children = d.successors(s);

        Hist bestShiftedHist;

        for (auto& c : children) {
            Cost h = d.heuristic(c);
            auto chist = findorShiftFromClosestHist(h);
            Cost hhat = chist.getMean();
            Cost edgeCost = d.getEdgeCost(c);
            hhat += edgeCost;
            if (hhat < backuphhat) {
                backuphhat = hhat;
                bestShiftedHist = chist.shift(edgeCost);
            }
        }

        return bestShiftedHist;
    }

    Hist mergeHists(vector<Hist>&& hists) const {
        Hist newHist;
        unordered_map<string, int> newHistKey;

        for (auto hist : hists) {
            for (auto bin : hist.getBins()) {
                if (newHistKey.find(bin.getValueString()) != newHistKey.end()) {
                    int binIndex = newHistKey[bin.getValueString()];
                    newHist.updateBin(binIndex, bin.getCount());
                }
				else{
                    newHistKey[bin.getValueString()] = newHist.getBins().size();
                    newHist.push(std::move(bin));
                }
            }
        }

		return newHist;
    }

    HStateMap hStatesCollection;
    std::unordered_map<std::string, Hist> originalhHist;
    std::vector<Cost> originalhValues;
    std::unordered_map<std::string, Hist> postSearchhHist;
};

class IOFiles {
public:
    IOFiles(string tileType) {
        if (tileType == "uniform") {
            sampleFile = "../results/SlidingTilePuzzle/sampleData/"
                         "uniform-samples.txt";
            distributionFile = "../results/SlidingTilePuzzle/sampleData/"
                               "uniform-statSummary.txt";
            postSearchFile = "../results/SlidingTilePuzzle/sampleData/"
                             "uniform-statSummary-postSearch.txt";

            instanceDir = "uniform";

            post_search_collection =
                    make_shared<PostSearchCollection<SlidingTilePuzzle>>();
        } else if (tileType == "inverse") {
            sampleFile = "../results/SlidingTilePuzzle/sampleData/"
                         "inverse_20_0.1_200-samples.txt";
            distributionFile = "../results/SlidingTilePuzzle/sampleData/"
                               "inverse-statSummary-20-0.1-200-backup.txt";
            postSearchFile = "../results/SlidingTilePuzzle/sampleData/"
                             "inverse-statSummary-20-0.1-200-postSearch.txt";

            instanceDir = "inverse-20-0.1-200";

            post_search_collection =
                    make_shared<PostSearchCollection<InverseTilePuzzle>>();
        }  else if (tileType == "heavy") {
            sampleFile = "../results/SlidingTilePuzzle/sampleData/"
                         "heavy-samples.txt";
            distributionFile = "../results/SlidingTilePuzzle/sampleData/"
                               "heavy-statSummary.txt";
            postSearchFile = "../results/SlidingTilePuzzle/sampleData/"
                             "heavy-statSummary-postSearch.txt";

            instanceDir = "heavy";

            post_search_collection =
                    make_shared<PostSearchCollection<HeavyTilePuzzle>>();
        } else {
            cout << "not support tile type" << tileType << endl;
            exit(1);
        }

        f_sample = std::ifstream(sampleFile);
        f_distribution = std::ifstream(distributionFile);
        f_out = std::ofstream(postSearchFile);
    }

    void run() {
        // read each line get all 200-ish samples
        post_search_collection->parsingSamples(f_sample, instanceDir);

        // for each sample problem, expand it find its succssors and their
        // h-values
        // query the h* distributions by the h-values
        // do nancy backup (eg, get the one with min f-hat), to get the
        // post-search
        // belief
        // for all 200-ish post-search beliefs, merge them by doing numerical
        // intergation
        // now we have the mapping that map h to post-search h*
        post_search_collection->computePostSearchHist(f_distribution);

        post_search_collection->dumpPostSearchHist(f_out);
    }

private:
    std::ifstream f_sample;
    std::ifstream f_distribution;
    std::ofstream f_out;

    shared_ptr<CollectionBase> post_search_collection;

    string sampleFile;
    string distributionFile;
    string postSearchFile;
    string instanceDir;
};

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Wrong number of arguments: ./collect-post-search <tile type>"
             << "\ntile type: uniform, inverse, heavy"
             << endl;
        exit(1);
    }

    IOFiles ioFiles(argv[1]);
    ioFiles.run();
}
