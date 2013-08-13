#ifndef CONTOURTREE_H_GUARD
#define CONTOURTREE_H_GUARD
#include <map>
#include "scalargraph.h"

class ContourTree : public ScalarGraph {
    public:
    void compute(ScalarGraph& sg);
    void addEdge(int,int);

    private:
    ScalarGraph joinTree(ScalarGraph&, std::vector<int>&, std::map<int,int>&);
    ScalarGraph splitTree(ScalarGraph&, std::vector<int>&, std::map<int,int>&);
    void mergeTree(ScalarGraph&, ScalarGraph&);
};

#endif
