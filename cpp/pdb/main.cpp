/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: main.cpp
 * description:main funciton for generating  patthern database for fifteen puzzle
 * date: March-25-2019
 */

#include "pdb.hpp"
#include <vector>

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Usage: ./pdb <pattern>\n";
        cout << "pattern: 7 or 8\n";
		exit(1);
    }

	PDB pdb;
	pdb.generateDisjointPDB(argv[1]);
}
