#pragma once
#include "utility/SearchResultContainer.h"

using namespace std;

class Search {
public:
    virtual SearchResultContainer search() = 0;

    virtual void dumpClosedList(ofstream& out) = 0;
};
