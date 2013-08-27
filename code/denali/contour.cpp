#include <iostream>
#include <iterator>
#include <string>
#include "scalargraph.h"
#include "contourtree.h"

int main() {

    // make the scalar graph on pg. 390 of Wenger
    ScalarGraph g;

    const int nodevalues[] = {25, 62, 45, 66, 16, 32, 64, 39, 58, 51, 53, 30};
    for (int i=0; i<12; ++i) {
        g.addNode(i+1, nodevalues[i]);
    }

    const int edges[][2] = {{1,2}, {2,3}, {4,5}, {5,6}, {7,8}, {8,9}, {10,11}, {11,12},
                            {1,4}, {4,7}, {7,10}, {2,5}, {5,8}, {8,11}, {3,6}, {6,9}, {9,12},
                            {1,5}, {2,6}, {4,8}, {5,9}, {7,11}, {8,12}};

    for (int i=0; i<sizeof(edges)/sizeof(edges[0]); ++i) {
        g.addEdge(edges[i][0], edges[i][1]);
    }

    ContourTree ct;
    ct.computeContourTree(g);
    ct.prettyPrint(std::cout);

    ct.setRoot(5);

    std::cout << "test";
    
}

