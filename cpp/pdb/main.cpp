/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: main.cpp
 * description:main funciton for generating  patthern database for fifteen puzzle
 * date: March-25-2019
 */

#include "pdb.hpp"
#include <vector>

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 1) {
        cout << "Usage: ./pdb\n";
		exit(1);
    }

    vector<int> pattern{7, 8};
	PDB pdb;
	pdb.generateDisjointPDB(pattern);
}
