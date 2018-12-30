#include <iostream>
#include <string>
#include <fstream>
#include "WAStarSearch.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc > 3) {
        cout << "Wrong number of arguments: ./distributionPractice.sh <weight> "
                "<optional: output file> < <domain file>"
             << endl;
        cout << "Domains is SlidingPuzzle, fixed" << endl;
        exit(1);
    }

    float weight = stof(argv[1]);

    SlidingTilePuzzle world = SlidingTilePuzzle(cin);

    WAStarSearch wastarsearch(world, weight);

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
}
