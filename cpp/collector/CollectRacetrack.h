#pragma once
#include "CollectBase.h"
#include <cmath>

template <class Domain>
class CollectionRacetrack : public Collection<Domain> {
    typedef typename Domain::State State;
    typedef typename Collection<Domain>::Node Node;

    void parsingDumpFiles(cxxopts::ParseResult& args) {
        int firstNum = args["first"].as<int>();
        int lastNum = args["last"].as<int>();
        string subdomain = args["subdomain"].as<string>();

        auto inputPath = inPath(args);

        for (int i = firstNum; i <= lastNum; i++) {
            string fileName = inputPath + to_string(i) + ".txt";
			//cout << "fileName:" << fileName << endl;

            ifstream f(fileName);

            parsingDumpFile(f, subdomain);

            f.close();
        }
    }

    void parsingDumpFile(ifstream& f, string& subdomain) {
        this->fileCount++;
        string line;

        getline(f, line);

        cout << "file #: " << this->fileCount << " ";
        cout << "file info: " << line << endl;

        while (getline(f, line)) {
            int x, y, dx, dy;
            std::string foo;
            stringstream ss(line);
            ss >> foo;
            ss >> x;
            ss >> foo;
            ss >> y;
            ss >> foo;
            ss >> dx;
			ss >> foo;
            ss >> dy;
            ss >> foo;

            getline(f, line);
            stringstream ss2(line);
			// nottice we are round h value here
            int h, d;
			double hdouble;
            unsigned long long key;
            ss2 >> hdouble;
			h = round(hdouble);
            ss2 >> d;
            ss2 >> key;

            if (this->nodeCollection.find(key) == this->nodeCollection.end()) {
                State s(x,y,dx,dy);
                shared_ptr<Node> n = make_shared<Node>(h, d, s);
                this->nodeCollection[key] = n;
            } else {
                this->nodeCollection[key]->frequencyCounter++;
            }
        }
	}

   string inPath(cxxopts::ParseResult& args) {
        string domain = args["domain"].as<string>();
        string subdomain = args["subdomain"].as<string>();
        string alg = args["alg"].as<string>();
        string algPara = args["par"].as<string>();
        return "../results/" + domain + "/distributionTest/" + alg + "/Para" +
                algPara + "-" + subdomain + "-";
    }

    string outPath(cxxopts::ParseResult& args) {
        string subdomain = args["subdomain"].as<string>();
        string domain = args["domain"].as<string>();
        string alg = args["alg"].as<string>();
        string algPara = args["par"].as<string>();

        return "../results/" + domain + "/sampleProblem/" + subdomain + "/" +
                alg + "/Para" + algPara;
    }
};

