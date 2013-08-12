#include "disjointsetforest.h"
#include <iostream>

using std::cout;

void printHeader(std::ostream& os, std::string title, char delim = '-') {
    os << std::endl;
    os << title << std::endl << std::string(title.size(), delim);
    os << std::endl;
}

bool x_before_y(int x, int y) {
    return x<y;
}

int main() {
    using std::cout; using std::endl;

    DisjointSetForest f;
    int n_sets = 10;

    printHeader(std::cout, "Making sets");
    for (int i=0; i<n_sets; ++i) {
        cout << "Making set: " << i << endl;
        f.makeSet(i);
    }
    

    printHeader(std::cout, "Unioning sets");
    cout << "Unioning 1 and 2" << endl;
    f.unionSets(0,1,x_before_y);
    f.unionSets(8,9,x_before_y);
    f.unionSets(7,9,x_before_y);
    f.unionSets(0,7,x_before_y);

    printHeader(std::cout, "Printing representatives");
    for (int i=0; i<n_sets; ++i) {
        cout << "Representative of " << i << ": " << f.findSet(i) << "\tMax: " << f.maxSet(i) << "\tMin: " << f.minSet(i) << endl;
    }

}
