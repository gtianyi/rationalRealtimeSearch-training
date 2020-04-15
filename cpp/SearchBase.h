#pragma once
#include "utility/SearchResultContainer.h"

using namespace std;

class Search {
public:
    virtual SearchResultContainer doSearch() = 0;

    virtual void dumpClosedList(ofstream& out) = 0;
};
