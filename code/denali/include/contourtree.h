#ifndef CONTOURTREE_H_GUARD
#define CONTOURTREE_H_GUARD
#include "scalargraph.h"
#include "common.h"

namespace denali {
void computeAugmentedContourTree(ScalarGraph&, ScalarGraph&);
void computeContourTree(ScalarGraph&, ScalarGraph&);
void removeRegularVertices(ScalarGraph&);
}

#endif
