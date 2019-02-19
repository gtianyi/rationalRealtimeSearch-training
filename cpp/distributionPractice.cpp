#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "WAStarSearch.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 4 || argc < 3) {
        cout << "Wrong number of arguments: ./distributionPractice <puzzle type> <weight> <optional: output file> < <domain file>"
             << endl;
        cout << "puzzle type: uniform, invers, heavy, sqrt" << endl;
        exit(1);
    }

	string puzzleType = argv[1];

    float weight = stof(argv[2]);

	 shared_ptr<SlidingTilePuzzle> world;
			
	 if(puzzleType == "uniform")
             world = make_shared<SlidingTilePuzzle>(cin);
	 else if(puzzleType == "heavy")
             world = make_shared<HeavyTilePuzzle>(cin);
	 else{
             cout << "wrong puzzle type: uniform, invers, heavy, sqrt" << endl;
             exit(1);
	 }

    WAStarSearch<SlidingTilePuzzle> wastarsearch(*world, weight);

    WAStarResultContainer wastarRes = wastarsearch.search();

    //PracticeResults practiceRes = wastarsearch.practice(wastarRes);

	//JSONObject json = new JSONObject(wastarRes);

	//string result = json.toString(2);

    if (argc < 4) {
        cout << wastarRes.nodesExpanded << " " << wastarRes.solutionFound << " "
             << wastarRes.solutionCost << " " << wastarRes.initialH << endl;
    } else {
        ofstream out(argv[3]);

        out << wastarRes.nodesExpanded << " " << wastarRes.solutionFound << " "
            << wastarRes.solutionCost << endl;

		wastarsearch.dumpClosedList(out);

        out.close();
    }

}
