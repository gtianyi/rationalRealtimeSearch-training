/* author: Tianyi Gu (gu@cs.unh.edu)
 * file: main.cpp
 * description:main funciton for generating  patthern database for fifteen puzzle
 * date: March-25-2019
 */

#include "pdb.hpp"
#include "heavyPartialTiles.hpp"
#include "inversePartialTiles.hpp"
#include "reversePartialTiles.hpp"
#include <string>
#include <vector>

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./pdb <pattern> <tileType>\n";
        cout << "pattern: 61 or 62\n";
        cout << "tile type: heavy or inverse\n";
		exit(1);
    }

    string subDomain = argv[2];

    if (subDomain == "heavy") {
        PDB<HeavyPartialTiles> pdb;
        pdb.generateDisjointPDB(argv[1]);

    } else if (subDomain == "inverse") {
        PDB<InversePartialTiles> pdb;
        pdb.generateDisjointPDB(argv[1]);
    } else if (subDomain == "reverse") {
        PDB<ReversePartialTiles> pdb;
        pdb.generateDisjointPDB(argv[1]);
    }

}
