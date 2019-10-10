#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "WAStarSearch.h"
#include "LsslrtastarSearch.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 5 || argc < 4) {
        cout << "Wrong number of arguments: ./distributionPractice <puzzle type> <alg> <alg arg> <optional: output file> < <domain file>"
             << endl;
        cout << "puzzle type: uniform, inverse, heavy, sqrt" << endl;
        cout << "alg: wastar, lsslrtastar" << endl;
        cout << "alg arg: wastar: weight, lsslrtastar: lookahead" << endl;
        exit(1);
    }

	string puzzleType = argv[1];
	
	string alg = argv[2];


    shared_ptr<SlidingTilePuzzle> world;

    if (puzzleType == "uniform")
        world = make_shared<SlidingTilePuzzle>(cin);
    else if (puzzleType == "heavy")
        world = make_shared<HeavyTilePuzzle>(cin);
    else if (puzzleType == "inverse")
        world = make_shared<InverseTilePuzzle>(cin);
    else {
        cout << "wrong puzzle type: uniform, inverse, heavy, sqrt" << endl;
        exit(1);
    }

    if (alg == "wastar") {
        float weight = stof(argv[3]);

        WAStarSearch<SlidingTilePuzzle> wastarsearch(*world, weight);

        WAStarResultContainer wastarRes = wastarsearch.search();

        // PracticeResults practiceRes = wastarsearch.practice(wastarRes);

        // JSONObject json = new JSONObject(wastarRes);

        // string result = json.toString(2);

        if (argc < 5) {
            cout << wastarRes.nodesExpanded << " " << wastarRes.solutionFound
                 << " " << wastarRes.solutionCost << " " << wastarRes.initialH
                 << endl;
        } else {
            ofstream out(argv[4]);

            out << wastarRes.nodesExpanded << " " << wastarRes.solutionFound
                << " " << wastarRes.solutionCost << endl;

            wastarsearch.dumpClosedList(out);

            out.close();
        }
    } else if (alg == "lsslrtastar") {
        int lookaheadDepth = stof(argv[3]);

        LssLRTAStarSearch<SlidingTilePuzzle> lsslrta(
                *world, "a-star", "learn", "minimin", lookaheadDepth);

        ResultContainer res = lsslrta.search();

        if (argc < 5) {
            cout << res.nodesExpanded << " " << res.solutionFound << " "
                 << res.solutionCost << "\n"
                 << endl;
        } else {
            ofstream out(argv[4]);

            out << res.nodesExpanded << " " << res.solutionFound << " "
                << res.solutionCost << endl;

            lsslrta.dumpClosedList(out);

            out.close();
        }
    }
}
