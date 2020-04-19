#include "../domain/SlidingTilePuzzle.h"
#include "../domain/PancakePuzzle.h"
#include "../utility/cxxopts/include/cxxopts.hpp"

#include "CollectBase.h"
#include "CollectTile.h"
#include "CollectPancake.h"

using namespace std;

int main(int argc, char** argv) {
    cxxopts::Options options("./collect", "This is program is for collecting "
                                          "obesrved states given the output "
                                          "files of an suboptimal serach");

    options.add_options()

		("d,domain", "domain type: randomtree, tile, pancake", 
		 cxxopts::value<string>()->default_value("pancake"))

		("s,subdomain", "puzzle type: uniform, inverse, heavy, sqrt; "
						"pancake type: regular, heavy; "
						"racetrack map : barto-bigger, hanse-bigger-double, uniform", 
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

		("l,last", "last instance", cxxopts::value<int>()->default_value("25"))

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

    if (domain == "tile") {
        collectionPtr = make_shared<CollectionTile<SlidingTilePuzzle>>();
    } else if (domain == "pancake") {
        collectionPtr = make_shared<CollectionPancake<PancakePuzzle>>();
    //} else if (domain == "racetrack") {
        //collectionPtr = make_shared<CollectionPancake<PancakePuzzle>>();
    } else {
        cout << "unknown domain!\n";
        exit(1);
	}

    string inPath = collectionPtr->inPath(args);
    string outPath = collectionPtr->outPath(args);

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
