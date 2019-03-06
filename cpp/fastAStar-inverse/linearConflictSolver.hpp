// Tianyi Gu
// Feb-27-2019
// Linear Conflict Solver

#include <cmath>
#include <unordered_set>
#include <queue>
#include <vector>
#include <algorithm>

 int getCompactIntByArray(int* v, int size) {
        int ret = 0;

        for (int i = 0; i < size; i++) {
            ret += (int)pow(10, size - 1 - i) * v[i];
        }

        return ret;
 };

using namespace std;

class LinearConflictSolver {
    struct Node {
        vector<int> row;
        double cost;
        int index;

        Node() {}

        Node(const Node& n) { deepCopy(n); }

        Node(vector<int>& row) : row(row), cost(0) {
            index = getCompactIntByArray(&row[0], row.size());
        }

        bool operator<(const Node& n) const { return cost > n.cost; }
        Node& operator=(const Node& n) {
            deepCopy(n);
            return *this;
        }

        void deepCopy(const Node& n) {
            row.clear();
            for (int i = 0; i < n.row.size(); i++) {
                row.push_back(n.row[i]);
            }
            cost = n.cost;
            index = n.index;
        }
    };

    void initial(vector<int>& row) {
        vector<int> goalRow = row;
        sort(goalRow.begin(), goalRow.end());
        goalIndex = getCompactIntByArray(&goalRow[0], goalRow.size());

        while (!open.empty()) {
            open.pop();
        }

        closed.clear();
    }

    bool isGoal(Node& n) { return n.index == goalIndex; }

    bool apply(Node& child, int action) {
        int start_pos = action / child.row.size();
        int target_pos = action % child.row.size();

        if (start_pos == target_pos) {
            return false;
        }

        int face = child.row[start_pos];

        if (start_pos > target_pos) {
            for (int i = start_pos; i > target_pos; i--) {
                child.row[i] = child.row[i - 1];
            }
        } else {
            for (int i = start_pos; i < target_pos; i++) {
                child.row[i] = child.row[i + 1];
            }
        }

        child.row[target_pos] = face;

        child.cost += 1.0 / (double)face;

        child.index = getCompactIntByArray(&child.row[0], child.row.size());

        if (closed.find(child.index) != closed.end())
            return false;

        return true;
    }

    unordered_set<int> closed;
    priority_queue<Node> open;
    int goalIndex;

public:
    double solve(vector<int>& row) {
        initial(row);

        Node n(row);
        open.push(n);

        while (!open.empty()) {
            Node cur = open.top();
            open.pop();

            if (isGoal(cur)) {
                return cur.cost;
            }

            for (int i = 0; i < row.size() * row.size(); i++) {
                Node child(cur);
                bool childValid = apply(child, i);
                if (childValid) {
                    open.push(child);
                }
            }

            closed.insert(cur.index);
        }

        return -1.0;
    }
};
