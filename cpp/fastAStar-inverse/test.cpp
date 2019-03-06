// Copyright 2012 Ethan Burns. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include "linearConflictSolver.hpp"
#include "fatal.hpp"
#include <cstring>
#include <vector>
#include <cassert>
#include <iostream>

using namespace std;

bool compare2double(double a, double b){
    return (int)(1000 * a) == (int)(1000 * b);
};

int main(int argc, const char *argv[]) {
    try {
        if (argc != 1)
            throw Fatal("Usage: tilestest");

        LinearConflictSolver solver;

        vector<int> r1 = {4, 5, 6, 7};
        vector<int> r2 = {4, 6, 5, 7};
        vector<int> r3 = {7, 6, 5, 4};
        vector<int> r4 = {5, 6, 4};
        vector<int> r5 = {6, 5, 4};
        vector<int> r6 = {5, 4};
        vector<int> r7 = {15, 14, 12};

        double c;

		c = solver.solve(r1);
		assert(compare2double(c, 0.0));
		cout << "pass r1\n";

		c = solver.solve(r2);
		assert(compare2double(c, 1.0 / 6.0));
		cout << "pass r2\n";

        c = solver.solve(r3);
        assert(compare2double(c, 1.0 / 7.0 + 1.0 / 5.0 + 1.0 / 6.0));
		cout<<"pass r3\n";

        c = solver.solve(r4);
        assert(compare2double(c , 1.0 / 4.0));;
		cout<<"pass r4\n";

        c = solver.solve(r5);
        assert(compare2double(c , 1.0 / 6.0 + 1.0 / 5.0));;
		cout<<"pass r5\n";

        c = solver.solve(r6);
        assert(compare2double(c , 1.0 / 5.0));;
        cout << "pass r6\n";

        c = solver.solve(r7);
        assert(compare2double(c, 1.0 / 15.0 + 1.0 / 14.0));
        cout << "pass r7\n";


    } catch (const Fatal& f) {
        fputs(f.msg, stderr);
        fputc('\n', stderr);
        return 1;
	}

	return 0;
}


