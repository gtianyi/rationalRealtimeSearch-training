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

    WAStarResult wastarRes = wastarsearch.search();

    PracticeResults practiceRes = wastarsearch.practice(wastarRes);

    JSONObject json = new JSONObject(practiceRes);

    string result = json.toString(2);

    if (argc < 2) {
        cout << result << endl;
    } else {
        ofstream out(argv[2]);

        out << result;
        out.close();
    }

	delete world;
}
