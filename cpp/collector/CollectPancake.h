#pragma once
#include "CollectBase.h"

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

        while (getline(f, line)) {
            vector<unsigned int> pcake(size, 0);
            stringstream ss(line);
            for (int p = 0; p < size; p++) {
                std::string pan;
                ss >> pan;
                pcake[p] = stoi(pan);
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

