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
constexpr int hsPrecision = 1;

template<class Domain, class hValueType>
class PostSearchCollection{
public:
    typedef typename Domain::State State;
    typedef typename Domain::Cost Cost;

    using HStateMap =
            std::unordered_map<std::string, std::vector<State>>;

    struct Hist {
        struct Bin {
            Bin(hValueType hs, int count) : value(hs), count(count) {
                valueString = double2string(hs, hsPrecision);
            }

            void setValue(hValueType v) {
                value = v;
                valueString = double2string(v, hsPrecision);
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

        vector<Bin> getBins() const { return bins; }

        double getMean() const { return mean; }

        int getTotalCount() const { return totalCount; }

    private:
        vector<Bin> bins;
        double mean;
        int totalCount;
    };

    typedef typename Hist::Bin Bin;

    State getStateByInstanceName(const string& instance) const {
        string fileName = "../results/SlidingTilePuzzle/sampleProblem/inverse-20-0.1-200/" + instance;
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

            while (ss >> instanceName) {
                statesList.push_back(getStateByInstanceName(instanceName));
            }

            assert(valueCount == statesList.size());

            hStatesCollection[h] = statesList;
        }

        f.close();
    }

    void parseHistData(ifstream& f) {
        string line;

		string hstring;
		hValueType hs, hvalue;
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

            assert(hist.getTotalCount() == valueCount);
			originalhHist[hstring]=hist;
			originalhValues.push_back(hvalue);
        }

		std::sort(originalhValues.begin(),originalhValues.end());

		f.close();
    }

    string findClosest(const Cost h) const{
        auto hString = double2string(h, hPrecision);

        if (originalhHist.find(hString) != originalhHist.end())
            return hString;

		Cost closesth;

        if (h > originalhValues[originalhValues.size() - 1])
            closesth =  originalhValues[originalhValues.size() - 1];
        else if (h < originalhValues[1])
		    // 0 is goal, we want to return the smallest h value for a non-goal state
            closesth = originalhValues[1];
		else {
				//perform binary search to find closest data point, 
				// or find 2, and merge the histgram
				// implement later, now it should cause segfault
				return "no found";
		}

		return double2string(closesth, hPrecision);
    }

    Hist lookahead(const State& s) const {
        Cost backuphhat = std::numeric_limits<double>::infinity();

        Domain d;

        auto children = d.successors(s);

        Hist bestShiftedHist;

        for (auto& c : children) {
            Cost h = d.heuristic(c);
            auto hString = findClosest(h);
            Cost hhat = originalhHist.at(hString).getMean();
            Cost edgeCost = d.getEdgeCost(c);
            hhat += edgeCost;
            if (hhat < backuphhat) {
                backuphhat = hhat;
                bestShiftedHist = originalhHist.at(hString).shift(edgeCost);
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

    void computePostSearchHist(ifstream& f) {
        parseHistData(f);

        for (auto it = hStatesCollection.begin(); it != hStatesCollection.end();
                it++) {
            std::vector<Hist> histListAfterBackup;

            for (auto s : it->second) {
				histListAfterBackup.push_back(lookahead(s));
            }

            postSearchhHist[it->first] =
                    mergeHists(std::move(histListAfterBackup));
        }
    }

    void dumpPostSearchHist(ofstream& f) const {
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
    HStateMap hStatesCollection;
    std::unordered_map<std::string, Hist> originalhHist;
    std::vector<hValueType> originalhValues;
    std::unordered_map<std::string, Hist> postSearchhHist;
};

int main(int argc, char** argv) {
    if (argc != 1) {
        cout << "Wrong number of arguments: ./collect-post-search"
             << endl;
        exit(1);
    }

    string sampleFile = "../results/SlidingTilePuzzle/sampleData/inverse_20_0.1_200-samples.txt";
    string distributionFile = "../results/SlidingTilePuzzle/sampleData/inverse-statSummary-20-0.1-200-backup.txt";
    string postSearchFile =
            "../results/SlidingTilePuzzle/sampleData/inverse-statSummary-20-0.1-200-postSearch.txt";
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
