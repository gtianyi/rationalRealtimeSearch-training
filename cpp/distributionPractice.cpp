#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <typeindex>
#include "WAStarSearch.h"
#include "LsslrtastarSearch.h"
#include "utility/cxxopts/include/cxxopts.hpp"

using namespace std;

struct BaseWorld {
    virtual void* getWorld(istream& mystdin) = 0;
};

template <typename Domain, typename Subdomain>
struct World :BaseWorld{
    void* getWorld(istream& mystdin) {
        shared_ptr<Domain> world = make_shared<Subdomain>(mystdin);
        return world;
    }
};

template<typename Domain, typename Algpara>
struct Search{
	auto getSearch(Domain& d, Algpara p) {
		if
			constexpr(std::is_same_v<Algpara, double>) {
				WAStarSearch<Domain> wastarsearch(d, p);
				return wastarsearch;
			}
		else if
			constexpr(std::is_same_v<Algpara, int>) {
				LssLRTAStarSearch<Domain> lsslrta(
						d, "a-star", "learn", "minimin", p);
				return lsslrta;
			}
		else {
			LssLRTAStarSearch<Domain> lsslrta(
					d, "a-star", "learn", "minimin", p);
			return lsslrta;
		}
	}
};



int main(int argc, char** argv) {
    cxxopts::Options options("./distributionPractice",
            "This is a suboptimal search to collect observed states");

    options.add_options()

		("d,domain", "domain type: randomtree, tile", 
		 cxxopts::value<std::string>()->default_value("randomtree"))

        ("s,subdomain", "puzzle type: uniform, inverse, heavy, sqrt", 
		 cxxopts::value<std::string>()->default_value("uniform"))

        ("a,alg", "suboptimal algorithm: wastar, lsslrtastar", 
		 cxxopts::value<std::string>()->default_value("lsslrtastar"))

        ("p,par", "weight for weighted A*, lookahead for lsslrta*", 
		 cxxopts::value<double>()->default_value("2.0"))

		("o,output", "output file", cxxopts::value<std::string>())

        ("h,help", "Print usage")
    ;

    auto args = options.parse(argc, argv);

	//=======================;

  /*  if (argc > 5 || argc < 4) {*/
        //cout << "Wrong number of arguments:  <puzzle type> <alg> <alg arg> < "
                //"optional : output file > < <domain file> "
             //<< endl;
        //cout << "" << endl;
        //cout << "alg: wastar, lsslrtastar" << endl;
        //cout << "" << endl;
        //exit(1);
	//}

	//string puzzleType = argv[1];
	
	//string alg = argv[2];
	//
    unordered_map<string, std::type_index> domains = {
            {"tile", typeid(SlidingTilePuzzle)},
            {"randomtree", typeid(TreeWorld)}};

    unordered_map<string, std::type_index> subdomains = {
            {"uniform", typeid(SlidingTilePuzzle)},
            {"heavy", typeid(HeavyTilePuzzle)},
            {"inverse", typeid(InverseTilePuzzle)}};

    unordered_map<string, std::type_index> algorithms = {
            {"wastar", typeid(double)}, {"lsslrtastar", typeid(int)}};

    auto d = args["domain"].as<std::string>();
    auto sd = args["subdomain"].as<std::string>();
    auto alg = args["alg"].as<std::string>();

	shared_ptr<BaseWorld> w;

    if (d == "randomtree") {
        w = make_shared<World<TreeWorld, TreeWorld>>();
		auto world = w->getWorld(cin);
        auto search = Search<TreeWorld, int>().getSearch(*world, args["par"].as<int>());
        ResultContainer res = search.search();
    } else if (d == "tile") {
        if (sd == "uniform") {
            auto world =
                    World<SlidingTilePuzzle, SlidingTilePuzzle>().getWorld(cin);
            if (alg == "wastar") {
                auto search = Search<SlidingTilePuzzle, double>().getSearch(
                        *world, args["par"].as<double>());

                WAStarResultContainer wastarRes = search.search();
            } else if (alg == "lssltrastar") {
                auto search = Search<SlidingTilePuzzle, int>().getSearch(
                        *world, args["par"].as<int>());

                ResultContainer res = search.search();
            }
        }
		
    }

    // auto res = search.search();

    // if (alg == "wastar") {
    // float weight = stof(argv[3]);

    // WAStarSearch<SlidingTilePuzzle> wastarsearch(*world, weight);

    // WAStarResultContainer wastarRes = wastarsearch.search();

    //// PracticeResults practiceRes = wastarsearch.practice(wastarRes);

    //// JSONObject json = new JSONObject(wastarRes);

    //// string result = json.toString(2);

    // if (argc < 5) {
    // cout << wastarRes.nodesExpanded << " " << wastarRes.solutionFound
    //<< " " << wastarRes.solutionCost << " " << wastarRes.initialH
    //<< endl;
    //} else {
    // ofstream out(argv[4]);

    // out << wastarRes.nodesExpanded << " " << wastarRes.solutionFound
    //<< " " << wastarRes.solutionCost << endl;

    // wastarsearch.dumpClosedList(out);

    // out.close();
    //}
    //} else if (alg == "lsslrtastar") {
    // int lookaheadDepth = stof(argv[3]);

    // LssLRTAStarSearch<SlidingTilePuzzle> lsslrta(
    //*world, "a-star", "learn", "minimin", lookaheadDepth);

    // ResultContainer res = lsslrta.search();

    // if (argc < 5) {
    // cout << res.nodesExpanded << " " << res.solutionFound << " "
    //<< res.solutionCost << "\n"
    //<< endl;
    //} else {
    // ofstream out(argv[4]);

    // out << res.nodesExpanded << " " << res.solutionFound << " "
    //<< res.solutionCost << endl;

    // lsslrta.dumpClosedList(out);

    // out.close();
    //}
    /*}*/
}
