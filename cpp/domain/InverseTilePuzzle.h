#pragma once
#include "SlidingTilePuzzle.h"

class InverseTilePuzzle : public SlidingTilePuzzle {
public:
    using SlidingTilePuzzle::SlidingTilePuzzle;

    Cost getEdgeCost(State state) { return 1.0 / (double)state.getFace(); }

    Cost heuristic(const State& state) {
        // Check if the heuristic of this state has been updated
        if (correctedH.find(state) != correctedH.end()) {
            return correctedH[state];
        }

        Cost h = manhattanDistanceWithInverseFaceCost(state);

        updateHeuristic(state, h);

        return correctedH[state];
    }

    Cost heuristic_no_recording(const State& state) {
        return manhattanDistanceWithInverseFaceCost(state);
    }

    Cost manhattanDistanceWithInverseFaceCost(const State& state) const {
        Cost manhattanSum = 0;

        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                auto value = state.getBoard()[r][c];
                if (value == 0) {
                    continue;
                }

                manhattanSum += (1.0 / (double)value) *
                        (abs(value / size - r) + abs(value % size - c));
                // cout << "value " << value << " sum " << manhattanSum << endl;
            }
        }

        return manhattanSum;
    }
};
