#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <typeindex>
#include "WAStarSearch.h"
#include "LsslrtastarSearch.h"
#include "utility/cxxopts/include/cxxopts.hpp"

using namespace std;

int main(int argc, char** argv) {

    cxxopts::Options options("./distributionPractice",
            "This is a suboptimal search to collect observed states");

    options.add_options()

		("d,domain", "domain type: randomtree, tile, pancake", 
		 cxxopts::value<std::string>()->default_value("pancake"))

        ("s,subdomain", "puzzle type: uniform, inverse, heavy, sqrt; pancake type: regular, heavy", 
		 cxxopts::value<std::string>()->default_value("regular"))

        ("a,alg", "suboptimal algorithm: wastar, lsslrtastar", 
		 cxxopts::value<std::string>()->default_value("wastar"))

        ("p,par", "weight for weighted A*, lookahead for lsslrta*", 
		 cxxopts::value<double>()->default_value("2"))

		("o,output", "output file", cxxopts::value<std::string>())

        ("h,help", "Print usage")
    ;

    auto args = options.parse(argc, argv);

    if (args.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
	}

    auto d = args["domain"].as<std::string>();
    auto sd = args["subdomain"].as<std::string>();
    auto alg = args["alg"].as<std::string>();
    auto para = args["par"].as<double>();

    SuboptimalSearch* searchPtr;

    // create domain world and search algorithm
    if (d == "randomtree") {
        TreeWorld* world = new TreeWorld(cin);
        auto lookahead = (int)para;
        searchPtr = new LssLRTAStarSearch<TreeWorld>(
                *world, "a-star", "learn", "minimin", lookahead);

    } else if (d == "tile") {
        SlidingTilePuzzle* world;

        if (sd == "uniform") {
            world = new SlidingTilePuzzle(cin);
        } else if (sd == "heavy") {
            world = new HeavyTilePuzzle(cin);
        } else if (sd == "inverse") {
            world = new InverseTilePuzzle(cin);
        } else {
            cout << "wrong tile type!\n";
            exit(1);
        }

        if (alg == "wastar") {
            auto weight = para;
            searchPtr = new WAStarSearch<SlidingTilePuzzle>(
                    *world, weight);
        } else if (alg == "lsslrtastar") {
            auto lookahead = (int)para;
            searchPtr = new LssLRTAStarSearch<SlidingTilePuzzle>(
                    *world, "a-star", "learn", "minimin", lookahead);
        }
    } else if (d == "pancake") {
        PancakePuzzle* world;

        world = new PancakePuzzle(cin);

        if (sd == "heavy") {
            world->setVariant(1);
        } 

        if (alg == "wastar") {
            auto weight = para;
            searchPtr = new WAStarSearch<PancakePuzzle>(*world, weight);
        } else if (alg == "lsslrtastar") {
            auto lookahead = (int)para;
            searchPtr = new LssLRTAStarSearch<PancakePuzzle>(
                    *world, "a-star", "learn", "minimin", lookahead);
        }
    } else {
        cout << "unknow domain!\n";
        std::cout << options.help() << std::endl;
        exit(0);
    }

    // perform search
    auto res = searchPtr->subOptSearch();

    // dumpout result and observed states
    if (args.count("output")) {
        ofstream out(args["output"].as<std::string>());

        out << res.nodesExpanded << " " << res.solutionFound << " "
            << res.solutionCost << endl;

        searchPtr->dumpClosedList(out);

        out.close();

    } else {
        cout << res.nodesExpanded << " " << res.solutionFound << " "
             << res.solutionCost << " " << res.initialH << endl;
    }
}
