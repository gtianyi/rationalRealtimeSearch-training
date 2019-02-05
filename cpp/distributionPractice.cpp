#include <iostream>
#include <string>
#include <fstream>
#include "WAStarSearch.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 3) {
        cout << "Wrong number of arguments: ./distributionPractice <puzzle type> <weight> <optional: output file> < <domain file>"
             << endl;
        cout << "puzzle type: uniform, invers, heavy, sqrt" << endl;
        exit(1);
    }

	string puzzleType = argv[1];

    float weight = stof(argv[2]);

    // SlidingTilePuzzle* world;
			
	// if(puzzleType == "uniform")
	 SlidingTilePuzzle	world(cin);

    WAStarSearch<SlidingTilePuzzle> wastarsearch(world, weight);

    WAStarResultContainer wastarRes = wastarsearch.search();

    //PracticeResults practiceRes = wastarsearch.practice(wastarRes);

	//JSONObject json = new JSONObject(wastarRes);

	//string result = json.toString(2);

    if (argc < 2) {
        cout << wastarRes.nodesExpanded << " " << wastarRes.solutionFound
             << endl;
    } else {
        ofstream out(argv[2]);

        out << wastarRes.nodesExpanded << " " << wastarRes.solutionFound;

        out.close();
    }

}
