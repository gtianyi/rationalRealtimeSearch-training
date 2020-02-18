#pragma once
#include <iostream>
#include "ResultContainer.h"

using namespace std;

struct SuboptSearchResultContainer : ResultContainer {
    double initialH;
};
