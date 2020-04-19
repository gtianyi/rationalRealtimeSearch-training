#pragma once
#include "../utility/PriorityQueue.h"
#include "../utility/SearchResultContainer.h"
#include "../domain/SlidingTilePuzzle.h"
#include "../domain/HeavyTilePuzzle.h"
#include "../domain/InverseTilePuzzle.h"
#include "../domain/TreeWorld.h"
#include "../domain/PancakePuzzle.h"
#include "../domain/RaceTrack.h"

#include "../expansionAlgorithms/IDAStar.h"
#include "../expansionAlgorithms/WAStar.h"


using namespace std;

class Search {
public:
    virtual SearchResultContainer doSearch() = 0;

    virtual void dumpClosedList(ofstream& out) = 0;
};
