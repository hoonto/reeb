#include "disjointsetforest.h"

void DisjointSetForest::makeSet(int id) {
    /* Make a new set in the forest */
    DisjointSet d;
    d.parent = id;
    d.max = id;
    d.min = id;
    d.rank = 0;
    id_to_set[id] = d;
}

int DisjointSetForest::findSet(int id) {
    /* Returns the representative of the set containing id. */
    DisjointSet& d = id_to_set[id];
    if (d.parent != id) {
        d.parent = findSet(d.parent);
    }
    return d.parent;
}

void DisjointSetForest::unionSets(int x, int y, std::map<int,int> enumeration) {
    /* Union two disjoint sets */
    // find the representative for each
    DisjointSet& d_x = id_to_set[x];
    DisjointSet& d_y = id_to_set[y];

    int rep_x_id = d_x.parent;
    int rep_y_id = d_y.parent;

    DisjointSet& rep_x = id_to_set[rep_x_id];
    DisjointSet& rep_y = id_to_set[rep_y_id];

    int max_id = enumeration[rep_x.max] < enumeration[rep_y.max] ? rep_y.max : rep_x.max;
    int min_id = enumeration[rep_x.min] < enumeration[rep_y.min] ? rep_x.min : rep_y.min;

    if (rep_x_id == rep_y_id) {
        return;
    } else {
        if (rep_x.rank > rep_y.rank) {
            rep_y.parent = rep_x_id;
            rep_x.max = max_id;
            rep_x.min = min_id;
        } else {
            rep_x.parent = rep_y_id;
            rep_y.max = max_id;
            rep_y.min = min_id;
            if (rep_x.rank == rep_y.rank) rep_y.rank++;
        }
    }
    return;
}

int DisjointSetForest::maxSet(int id) {
    /* Returns the id of the maximum member of the set. */
    int rep_id = findSet(id);
    return id_to_set[rep_id].max;
}

int DisjointSetForest::minSet(int id) {
    /* Returns the id of the minimum member of the set. */
    int rep_id = findSet(id);
    return id_to_set[rep_id].min;
}
