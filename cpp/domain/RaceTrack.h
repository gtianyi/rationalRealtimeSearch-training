#pragma once
#include <algorithm>
#include <iomanip>
#include <limits>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <cmath>
#include "../utility/SlidingWindow.h"

#include <bitset>

using namespace std;

struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator () (std::pair<T1, T2> const &pair) const
	{
		std::size_t h1 = std::hash<T1>()(pair.first);
		std::size_t h2 = std::hash<T2>()(pair.second);

		return h1 ^ h2;
	}
};


class RaceTrack {
    using Location = pair<int, int>;

public:
    typedef double Cost;
    static constexpr Cost COST_MAX = std::numeric_limits<Cost>::max();

    class State {
    public:
        State() {}

        State(int x, int y, int dx, int dy) : x(x),y(y) ,dx(dx) ,dy(dy) {
            generateKey();
        }

        friend std::ostream& operator<<(std::ostream& stream,
                const RaceTrack::State& state) {
            stream << "x: " << state.x << " ";
            stream << "y: " << state.y << " ";
            stream << "dx: " << state.dx << " ";
            stream << "dy: " << state.dy << "\n";
            return stream;
        }

        bool operator==(const State& state) const {
            return theKey == state.key();
        }

        bool operator!=(const State& state) const {
            return theKey != state.key();
        }

        void generateKey() {
            // This will provide a unique hash for every state in the 15 puzzle,
            // Other puzzle variants may/will see collisions...
            unsigned long long val = 0xCBF29CE484222325;
            val = val ^ x ^ (dx << 8) ^ (y << 16) ^ (dy << 24);
            theKey = val;
        }

        unsigned long long key() const { return theKey; }

        int getX() const { return x; }

        int getY() const { return y; }

        int getDX() const { return dx; }

        int getDY() const { return dy; }

        std::string toString() const {
            std::string s = to_string(x) + " " + to_string(y) + " " +
                    to_string(dx) + " " + to_string(dy) + "\n";
            return s;
        }

        void dumpToProblemFile(ofstream& f) {
            f << "x y dx dy for racetrack:\n";
			f<<x<<" "<<y<<" "<<dx<<" "<<dy<<"\n";
            f << "goal positions are in the map:\n";
		}

        void markStart() { label = 's'; }

    private:
        int x, y, dx, dy;
        char label;
        unsigned long long theKey = -1;
    };

    struct HashState {
        std::size_t operator()(const State& s) const { return s.key(); }

		// not sure if this necessary
        /*std::size_t leftRotate(std::size_t n, unsigned int d) const {*/
            //return (n << d) | (n >> (32 - d));
        /*}*/
    };

    void parseMap(std::istream& input) {
        string line;
        getline(input, line);
        stringstream ss(line);
        ss >> mapWidth;

        getline(input, line);

        stringstream ss2(line);
        ss2 >> mapHeight;

        for (int y = 0; y < mapHeight; y++) {

            getline(input, line);
            stringstream ss3(line);

            for (int x = 0; x < mapWidth; x++) {
                char cell;
                ss3 >> cell;

                switch (cell) {
                case '#':
                    blockedCells.insert(Location(x, y));
					break;
                case '*':
                    finishline.insert(Location(x, y));
					break;
                case '@':
                    startLocation = Location(x, y);
					break;
                }
            }
        }

		maxXSpeed = mapWidth/2;
		maxYSpeed=mapHeight/2;
		maxSpeed=max(maxXSpeed,maxYSpeed);

        startState = State(startLocation.first, startLocation.second, 0, 0);
        cout << "size: " << mapWidth << "x" << mapHeight << "\n";
        cout << "blocked: " << blockedCells.size() << "\n";
        cout << "finish: " << finishline.size() << "\n";
        cout << "start" << startState << "\n";
    }

    void initilaizeActions() {
        actions = {make_pair(-1, 1),
                make_pair(0, 1),
                make_pair(1, 1),
                make_pair(-1, 0),
                make_pair(0, 0),
                make_pair(1, 0),
                make_pair(-1, -1),
                make_pair(0, -1),
                make_pair(1, -1)};
    }

    void resetInitialState(std::istream& input) {
        string line;
		//skip the first line
        getline(input, line);
        getline(input, line);
        stringstream ss(line);
        int x, y, dx, dy;
        ss >> x;
        ss >> y;
        ss >> dx;
        ss >> dy;

        startState = State(x, y, dx, dy);
    }

    RaceTrack(std::istream& input, std::istream& initialState) {
        parseMap(input);
        resetInitialState(initialState);
		initilaizeActions();
    }

    RaceTrack(std::istream& input) {
        parseMap(input);
        initilaizeActions();
    }

    bool isGoal(const State& s) const {
        Location loc = Location(s.getX(), s.getY());
        return finishline.find(loc) != finishline.end();
    }



    Cost distance(const State& state) {
        // Check if the distance of this state has been updated
        if (correctedD.find(state) != correctedD.end()) {
            return correctedD[state];
        }

        Cost d = maxH(state);

        updateDistance(state, d);

        return correctedD[state];
    }

    Cost distanceErr(const State& state) {
        // Check if the distance error of this state has been updated
        if (correctedDerr.find(state) != correctedDerr.end()) {
            return correctedDerr[state];
        }

        Cost derr = maxH(state);

        updateDistanceErr(state, derr);

        return correctedDerr[state];
    }

    virtual Cost heuristic(const State& state) {
        // Check if the heuristic of this state has been updated
        if (correctedH.find(state) != correctedH.end()) {
            return correctedH[state];
        }

        Cost h = maxH(state);

        updateHeuristic(state, h);

        return correctedH[state];
    }

    virtual Cost heuristic_no_recording(const State& state) {
      return  maxH(state);
    }


    Cost epsilonHGlobal() { return curEpsilonH; }

    Cost epsilonDGlobal() { return curEpsilonD; }

    void updateEpsilons() {
        if (expansionCounter == 0) {
            curEpsilonD = 0;
            curEpsilonH = 0;

            return;
        }

        curEpsilonD = epsilonDSum / expansionCounter;

        curEpsilonH = epsilonHSum / expansionCounter;
    }

    void pushEpsilonHGlobal(double eps) {
        if (eps < 0)
            eps = 0;
        else if (eps > 1)
            eps = 1;

        epsilonHSum += eps;
        expansionCounter++;
    }

    void pushEpsilonDGlobal(double eps) {
        if (eps < 0)
            eps = 0;
        else if (eps > 1)
            eps = 1;

        epsilonDSum += eps;
        expansionCounter++;
    }

    void updateDistance(const State& state, Cost value) {
        correctedD[state] = value;
    }

    void updateDistanceErr(const State& state, Cost value) {
        correctedDerr[state] = value;
    }

    void updateHeuristic(const State& state, Cost value) {
        correctedH[state] = value;
    }

    Cost maxT(const State& startState, const Location& endLoc) const {
        return max(abs(startState.getX() - endLoc.first) / maxXSpeed,
                abs(startState.getY() - endLoc.second) / maxYSpeed);
    }

    Cost maxH(const State& state) const {
        Cost c = COST_MAX;

        for (const auto goalLoc : finishline) {
            auto newC = maxT(state, goalLoc);
            if (c > newC)
                c = newC;
        }

        return c;
    }

    double getBranchingFactor() const { return 9; }

    bool isLegalLocation(int x, int y) const {
        return x >= 0 && y >= 0 && x < mapWidth && y < mapHeight &&
                blockedCells.find(Location(x, y)) == blockedCells.end();
    }

    bool isCollisionFree(int x, int y, int dx, int dy) const {
        double distance =
                round(sqrt(pow((double)dx, 2.0) + pow((double)dy, 2.0)));

        double xRunning = double(x);
        double yRunning = double(y);

        double dt = 1.0 / distance;
        bool valid = true;

        double stepX = dx * dt;
        double stepY = dy * dt;

        for (int i = 1; i <= (int)distance; i++) {
            xRunning += stepX;
            yRunning += stepY;

            if (!isLegalLocation((int)round(xRunning), (int)round(yRunning))) {
                valid = false;
                break;
            }
        }

        return valid;
    }

    std::vector<State> successors(const State& state) const {
        std::vector<State> successors;

        for (auto action : actions) {
            int newDX = state.getDX() + action.first;
            int newDY = state.getDY() + action.second;

			// check if dx, dy already 0?
			// otherwise will alway stay? 
			// would it prefer to move so that be closer to goal
            if (newDX == 0 && newDY == 0) {
                successors.push_back(
                        State(state.getX(), state.getY(), newDX, newDY));
                continue;
            }

            if (isCollisionFree(state.getX(), state.getY(), newDX, newDY)) {
                successors.push_back(State(state.getX() + newDX,
                        state.getY() + newDY,
                        newDX,
                        newDY));
            }
        }

        return successors;
    }

    std::vector<State> predecessors(const State& state) const {
        std::vector<State> predecessors;

        //TODO not sure how to do this yet

        return predecessors;
    }

    bool safetyPredicate(const State& state) const { return true; }

    const State getStartState() const { return startState; }

    virtual Cost getEdgeCost(State state) { return 1; }

    string getDomainInformation() {
        string info =
                "{ \"Domain\": \"race track\", \"widthxheight\": " +
                std::to_string(mapHeight) + "x" + std::to_string(mapHeight) + " }";
        return info;
    }

    string getDomainName() { return "RaceTrack"; }

    void initialize(string policy, int la) {
        epsilonDSum = 0;
        epsilonHSum = 0;
        expansionCounter = 0;
        curEpsilonD = 0;
        curEpsilonH = 0;

        expansionPolicy = policy;
        lookahead = la;
        correctedD.clear();
        correctedH.clear();
        correctedDerr.clear();
        expansionDelayWindow.clear();
    }

    void pushDelayWindow(int val) { expansionDelayWindow.push(val); }

    double averageDelayWindow() {
        if (expansionDelayWindow.size() == 0)
            return 1;

        double avg = 0;

        for (auto i : expansionDelayWindow) {
            avg += i;
        }

        avg /= expansionDelayWindow.size();

        return avg;
    }

    std::unordered_set<Location,pair_hash> blockedCells;
    std::unordered_set<Location,pair_hash> finishline;
	vector<pair<int,int>> actions;
    int mapWidth;
    int mapHeight;
	double maxXSpeed;
	double maxYSpeed;
	double maxSpeed;
	Location startLocation;
    State startState;

    SlidingWindow<int> expansionDelayWindow;
    unordered_map<State, Cost, HashState> correctedH;
    unordered_map<State, Cost, HashState> correctedD;
    unordered_map<State, Cost, HashState> correctedDerr;

    double epsilonHSum;
    double epsilonDSum;
    double curEpsilonH;
    double curEpsilonD;
    double expansionCounter;

    string expansionPolicy;
    int lookahead;
};
