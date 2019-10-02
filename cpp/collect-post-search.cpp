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
#include "utility/rapidjson/document.h"
#include "utility/rapidjson/writer.h"
#include "utility/rapidjson/stringbuffer.h"
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

//TODO change this to be not hard coded
//uniform and heavy
//constexpr int hPrecision = 0;
//constexpr int hsPrecision = 0;
//constexpr double hSearchDownStep = 1;

//inverse
constexpr int hPrecision = 1;
constexpr int hsPrecision = 2;
constexpr double hSearchDownStep = 0.1;

struct CollectionBase{
    virtual void parsingSamples(ifstream& f, const string& instanceDir)=0; 
    virtual void computePostSearchHist(ifstream& f)=0;
    virtual void dumpPostSearchSamples(ofstream& f) const = 0;
};

template <class Domain>
class PostSearchCollection : public CollectionBase {
public:
  virtual void parsingSamples(ifstream& f,
            const string& instanceDir) override {
        if (!f.good()) {
            cout << "sample file does not exist!!!i\n";
            exit(1);
        }

        string jsonStr;
        getline(f, jsonStr);
        f.close();
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(jsonStr.c_str());

        for (auto& m : jsonDoc.GetObject()) {

            string h = m.name.GetString();
            vector<Sample> sampleList;

            for (auto& instance : m.value.GetArray()) {

                string instanceName = instance["instance"].GetString();
                int counter = instance["counter"].GetInt();
                bool isFloat = instanceDir == "inverse";
                Cost hstar = isFloat ? instance["hstar"].GetFloat() :
                                       instance["hstar"].GetInt();
                Cost deltaH = isFloat ? instance["deltaH"].GetFloat() :
                                        instance["deltaH"].GetInt();

                RawState s = getStateByInstanceName(instanceName, instanceDir);
				sampleList.push_back(Sample(s,deltaH,counter,hstar, instanceName));
			}

            hSampleCollection[h] = sampleList;
        }
    }

    virtual void computePostSearchHist(ifstream& f) override {
        parseHistData(f);

        for (auto it = hSampleCollection.begin(); it != hSampleCollection.end();
                it++) {
            for (auto& s : it->second) {
                 const auto& samples = lookahead(s.rawS, s.deltaH);
                 postSearchCollection[it->first].insert(
                         postSearchCollection[it->first].end(),
                         samples.begin(),
                         samples.end());
            }
        }
    }

    bool isFloat(const string& s) const {
        float tempfloat;
        istringstream iss(s);
        iss >> tempfloat;
        return !iss.fail();
	}

    virtual void dumpPostSearchSamples(ofstream& f) const override{
        // sort by h before dump
        vector<string> keys;

		bool isFloatH = false;

        for (auto& it : postSearchCollection) {
            keys.push_back(it.first);

            if (!isFloatH) {
                isFloatH = isFloat(it.first);
            }
        }

		sort(keys.begin(),keys.end());

		//dump to json

        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        writer.StartObject(); 

        for (auto& h : keys) {
            writer.Key(h.c_str()); 
            writer.StartArray();

            for (auto& s : postSearchCollection.at(h)) {
                writer.StartObject();

                writer.Key("instance");
                writer.String(s.instanceName.c_str());
                writer.Key("counter");
                writer.Uint(s.freqCounter);

                if (isFloatH) {

                    writer.Key("hstar");
                    writer.Double(s.hstar);
                    writer.Key("deltaH");
                    writer.Double(s.deltaH);

                } else {

                    writer.Key("hstar");
                    writer.Uint(s.hstar);
                    writer.Key("deltaH");
                    writer.Uint(s.deltaH);

                }

                writer.EndObject();
            }
            writer.EndArray();
        }

        writer.EndObject();

        f << s.GetString() << endl;

		f.close();
    }

private:

    typedef typename Domain::State RawState;
    typedef typename Domain::Cost Cost;

    struct Sample {
        RawState rawS;
        // h of missing - h of the state
		// eg: h = 3 have no data
		//     borow one instance from h = 5
		//     delta h = 3-5 = -2
        Cost deltaH;
        int freqCounter;
        Cost hstar;
		string instanceName;

        Sample() = delete;
        Sample(RawState& s, Cost deltaH, int counter, Cost hs, string& name)
                : rawS(s),
                  deltaH(deltaH),
                  freqCounter(counter),
                  hstar(hs),
                  instanceName(name){};

		Sample(const Sample& rhs, Cost shift)
                : rawS(rhs.rawS),
                  deltaH(rhs.deltaH),
                  freqCounter(rhs.freqCounter),
                  hstar(rhs.hstar+shift),
                  instanceName(rhs.instanceName){};
    };

    using HSampleMap = std::unordered_map<std::string, std::vector<Sample>>;

    struct Hist {
        struct Bin {
            Bin(Cost hs, float prob) : value(hs), prob(prob) {
                valueString = double2string(hs, hsPrecision);
            }

            void setValue(Cost v) {
                value = v;
                valueString = double2string(v, hsPrecision);
            }

            Cost getValue() const { return value; }
            string getValueString() const { return valueString; }

            void setProb(float p) { prob = p; }
            int getProb() const { return prob; }
            //void addCount(int c) { count += c; }

            void print(ofstream& f) const {
                f << valueString << " " << prob << " ";
            }

        private:
            Cost value;
            string valueString;
            float prob;
        };

        Hist() : mean(0), totalProb(0) {}

        void push(Bin&& bin) {
            mean += bin.getValue() * bin.getProb();
            totalProb += bin.getProb();
            bins.push_back(std::move(bin));
        }

        Hist shift(Cost c) const {
            Hist retHist;

            retHist.mean = mean + c;
            retHist.totalProb = totalProb;

            for (auto bin : bins) {
                Bin newBin(bin.getValue() + c, bin.getProb());
                retHist.bins.push_back(newBin);
            }

            return retHist;
        }

        vector<Bin> getBins() const { return bins; }

        double getMean() const { return mean; }

    private:
        vector<Bin> bins;
        double mean;
        float totalProb;
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

        if (!f.good()) {
            cout << "Hist Data file does not exist!!!i\n";
            exit(1);
        }

        string jsonStr;
        getline(f, jsonStr);
        f.close();
        rapidjson::Document jsonDoc;
        jsonDoc.Parse(jsonStr.c_str());

        for (auto& m : jsonDoc.GetObject()) {
            auto hstring = m.name.GetString();
            Cost h = stod(hstring);

            Hist hist;
            auto& bins = m.value.GetObject()["bins"];
            for (auto& instance : bins.GetArray()) {

                Cost hstar = isFloat(hstring) ? instance["h*"].GetFloat() :
                                                instance["h*"].GetInt();
                Cost prob = stod(instance["prob"].GetString());

                hist.push(Bin(hstar, prob));
            }

            originalhHist[hstring] = hist;
            originalhValues.push_back(h);
        }

        std::sort(originalhValues.begin(), originalhValues.end());

        f.close();
    }

    vector<Sample> lookahead(const RawState& s, const Cost deltaH) const {
		// s: the raw sample state
		// deltaH, the shift cost if this sample is a borrowed sample
        Cost backuphhat = std::numeric_limits<double>::infinity();

        Domain d;

        Cost h = d.heuristic(s);

        vector<Sample> ret;
        if (h == 0) {
            for (const auto& s :
                    hSampleCollection.at(double2string(h, hPrecision))) {
                ret.push_back(Sample(s, deltaH));
            }
            return ret;
        }

        auto children = d.successors(s);

        Cost bestChildH;
        Cost bestChildShift;
        Cost bestChildEdgeCost;

        for (auto& c : children) {
            Cost h = d.heuristic(c);
            Cost closesth = h;
            auto chist = findorShiftFromClosestHist(h, closesth);
            Cost hhat = chist.getMean();
            Cost edgeCost = d.getEdgeCost(c);
            hhat += edgeCost;
            if (hhat < backuphhat) {
                backuphhat = hhat;
                bestChildH = closesth;
                bestChildShift = h - closesth;
                bestChildEdgeCost = edgeCost;
            }
        }

        for (const auto& s :
                hSampleCollection.at(double2string(bestChildH, hPrecision))) {
            ret.push_back(Sample(s, bestChildEdgeCost + bestChildShift + deltaH));
        }

        return ret;
    }

    Hist findorShiftFromClosestHist(const Cost h, Cost& closesth) const {
        auto hString = double2string(h, hPrecision);

        int steps = 0;

        if (originalhHist.find(hString) != originalhHist.end()) {
            return originalhHist.at(hString);
        } else {

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
                        closesth = curH;
                        break;
                    }
                }
            }
        }

        return originalhHist.at(hString).shift((Cost)steps * hSearchDownStep);
    }

    HSampleMap hSampleCollection;
    std::unordered_map<std::string, Hist> originalhHist;
    std::vector<Cost> originalhValues;
    HSampleMap postSearchCollection;
};

class IOFiles {
public:
    IOFiles(string tileType) {
        if (tileType == "uniform") {
            sampleFile = "../results/SlidingTilePuzzle/sampleData/"
                         "uniform-samples.json";
            distributionFile = "../results/SlidingTilePuzzle/sampleData/"
                               "uniform-statSummary-d.json";
            postSearchFile = "../results/SlidingTilePuzzle/sampleData/"
                             "uniform-samples-postSearch.json";

            instanceDir = "uniform";

            post_search_collection =
                    make_shared<PostSearchCollection<SlidingTilePuzzle>>();
        } else if (tileType == "inverse") {
            sampleFile = "../results/SlidingTilePuzzle/sampleData/"
                         "inverse-samples.json";
            distributionFile = "../results/SlidingTilePuzzle/sampleData/"
                               "inverse-statSummary-d.json";
            postSearchFile = "../results/SlidingTilePuzzle/sampleData/"
                             "inverse-samples-postSearch.json";

            instanceDir = "inverse";

            post_search_collection =
                    make_shared<PostSearchCollection<InverseTilePuzzle>>();
        }  else if (tileType == "heavy") {
            sampleFile = "../results/SlidingTilePuzzle/sampleData/"
                         "heavy-samples.json";
            distributionFile = "../results/SlidingTilePuzzle/sampleData/"
                               "heavy-statSummary-d.json";
            postSearchFile = "../results/SlidingTilePuzzle/sampleData/"
                             "heavy-samples-postSearch.json";

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

        post_search_collection->dumpPostSearchSamples(f_out);
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
