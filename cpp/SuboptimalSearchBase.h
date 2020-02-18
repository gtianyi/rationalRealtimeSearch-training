#pragma once
#include "utility/SuboptimalSearchRresultContainer.h"

using namespace std;

class SuboptimalSearch {
public:
    virtual SuboptSearchResultContainer subOptSearch() = 0;

    virtual void dumpClosedList(ofstream& out) = 0;
};
