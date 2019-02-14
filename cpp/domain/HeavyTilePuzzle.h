#pragma once
#include "SlidingTilePuzzle.h"

class HeavyTilePuzzle : public SlidingTilePuzzle {
public:
    using SlidingTilePuzzle::SlidingTilePuzzle;
    Cost getEdgeCost(State state) { return state.getFace(); }
};
