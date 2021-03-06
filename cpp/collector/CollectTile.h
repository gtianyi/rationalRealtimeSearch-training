#pragma once
#include "CollectBase.h"

template <class Domain>
class CollectionTile : public Collection<Domain> {
    typedef typename Domain::State State;
    typedef typename Collection<Domain>::Node Node;

    void parsingDumpFiles(cxxopts::ParseResult& args) {
        int firstNum = args["first"].as<int>();
        int lastNum = args["last"].as<int>();
        string size = args["size"].as<string>();

        auto inputPath = inPath(args);

        for (int i = firstNum; i <= lastNum; i++) {
            string fileName = inputPath + to_string(i) + ".txt";

            ifstream f(fileName);

            parsingDumpFile(f, size);

            f.close();
        }
    }

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

    string inPath(cxxopts::ParseResult& args) {
        string subdomain = args["subdomain"].as<string>();
        string alg = args["alg"].as<string>();
        string algPara = args["par"].as<string>();
        return "../results/SlidingTilePuzzle/distributionTest/" +
                subdomain + "/" + alg + "/W" + algPara + "-";
    }

    string outPath(cxxopts::ParseResult& args) {
        string subdomain = args["subdomain"].as<string>();
        string alg = args["alg"].as<string>();

        return "../results/SlidingTilePuzzle/sampleProblem/" + subdomain + "/" +
                alg;
    }
};
