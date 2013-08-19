#include <map>
#include <vector>
#include <queue>
#include <list>
#include <stdexcept>
#include <iterator>
#include "scalargraph.h"
#include "contourtree.h"
#include "disjointsetforest.h"
#include "common.h"


void ContourTree::computeJoinTree(ScalarGraph& sg, std::vector<NodeID>& order_to_id,
        std::map<NodeID,NodeID>& id_to_order) {
    join_tree.clear();
    DisjointSetForest forest;

    // iterate through the vertices in ascending order
    for (std::vector<NodeID>::iterator it_i=order_to_id.begin(); it_i!=order_to_id.end(); ++it_i) {
        NodeID vi = *it_i;
        join_tree.addNode(vi, sg.getValue(vi));
        forest.makeSet(vi);
        
        // iterate through each of the neighbors
        std::list<NodeID> neighbors = sg.getNeighbors(vi);
        for (std::list<NodeID>::iterator it_j=neighbors.begin(); it_j!=neighbors.end(); ++it_j) {
            // obtain the node's ID
            NodeID vj = *it_j;
            // get the position of the nodes in the ordering
            NodeID i = id_to_order[vi]; 
            NodeID j = id_to_order[vj];
            // get the representative ids of each set
            NodeID rep_i = forest.findSet(vi); 
            NodeID rep_j = forest.findSet(vj);
            
            if (j<i && rep_i!=rep_j) {
                // get the max member of the set containing vj
                NodeID vk = forest.maxSet(vj);
                // make vk the parent of vj in the join tree
                join_tree.addEdge(vi,vk);
                // union the sets
                forest.unionSets(vi,vj,id_to_order);
            }
        }
    }
}


void ContourTree::computeSplitTree(ScalarGraph& sg, std::vector<NodeID>& order_to_id,
        std::map<NodeID,NodeID>& id_to_order) {
    split_tree.clear();
    DisjointSetForest forest;

    // iterate through the vertices in reverse order
    for (std::vector<NodeID>::reverse_iterator it_i=order_to_id.rbegin(); it_i!=order_to_id.rend(); ++it_i) {
        NodeID vi = *it_i;
        split_tree.addNode(vi, sg.getValue(vi));
        forest.makeSet(vi);

        // iterate through each of the neighbors
        std::list<NodeID> neighbors = sg.getNeighbors(vi);
        for (std::list<NodeID>::iterator it_j=neighbors.begin(); it_j!=neighbors.end(); ++it_j) {
            // obtain the node's ID
            NodeID vj = *it_j;
            // get the position of the nodes in the ordering
            NodeID i = id_to_order[vi]; 
            NodeID j = id_to_order[vj];
            // get the representative ids of each set
            NodeID rep_i = forest.findSet(vi); 
            NodeID rep_j = forest.findSet(vj);

            if (j>i && rep_i!=rep_j) {
                // get the max member of the set containing vj
                NodeID vk = forest.minSet(vj);
                // make vk the parent of vj in the join tree
                split_tree.addEdge(vi,vk);
                // union the sets
                forest.unionSets(vi,vj,id_to_order);
            }
        }
    }
}


void ContourTree::mergeTrees() {
    is_rooted = false;
    contour_tree.clear();
    std::vector<NodeID> nodes = join_tree.getNodes();  
    std::queue<NodeID> merge_queue;

    // initialize the tree with vertices and add to the merge queue
    for (std::vector<NodeID>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
        NodeID ai = *it;
        // create the vertex in the contour tree
        contour_tree.addNode(ai, join_tree.getValue(ai));
        if (join_tree.getNumberOfChildren(ai) + split_tree.getNumberOfChildren(ai) == 1)
            merge_queue.push(ai);
    }

    while (merge_queue.size()>1) {
        // get the value at the front of the queue
        NodeID& vi = merge_queue.front();
        
        if (join_tree.getNumberOfChildren(vi) == 0) {
            // get the predecessor of this node in the join tree
            NodeID vk = join_tree.getPredecessors(vi).front();
            // make vk the parent of vi in the contour tree
            contour_tree.addEdge(vk,vi);
            // delete the node from the join tree
            join_tree.removeNode(vi);
            // connect vi's parent to vi's child in the split tree
            if (split_tree.getNumberOfPredecessors(vi) > 0) {
                NodeID split_predecessor = split_tree.getPredecessors(vi).front();
                NodeID split_child = split_tree.getChildren(vi).front();
                split_tree.addEdge(split_predecessor, split_child);
            }
            split_tree.removeNode(vi);
            // add to the queue if necessary
            if (join_tree.getNumberOfChildren(vk) + split_tree.getNumberOfChildren(vk) == 1) {
                merge_queue.push(vk);
            }
        } else {
            // get the predecessor of this node in the join tree
            NodeID vk = split_tree.getPredecessors(vi).front();
            // make vk the parent of vi in the contour tree
            contour_tree.addEdge(vk,vi);
            // delete the node from the join tree
            split_tree.removeNode(vi);
            // connect vi's parent to vi's child in the split tree
            if (join_tree.getNumberOfPredecessors(vi) > 0) {
                NodeID join_predecessor = join_tree.getPredecessors(vi).front();
                NodeID join_child = join_tree.getChildren(vi).front();
                join_tree.addEdge(join_predecessor, join_child);
            }
            join_tree.removeNode(vi);
            // add to the queue if necessary
            if (join_tree.getNumberOfChildren(vk) + split_tree.getNumberOfChildren(vk) == 1) {
                merge_queue.push(vk);
            }
        }
        merge_queue.pop();
    }
}


void ContourTree::computeAugmentedContourTree(ScalarGraph& plex) {
    
    // get the sorted nodes
    std::vector<NodeID> order_to_id = plex.getSortedNodes();

    // make the reverse mapping from id to order
    std::map<NodeID,NodeID> id_to_order;
    for (NodeID order=0; order<order_to_id.size(); ++order) {
        id_to_order[order_to_id[order]] = order;
    }

    ScalarGraph join_tree, split_tree;
    computeJoinTree(plex, order_to_id, id_to_order);
    computeSplitTree(plex, order_to_id, id_to_order);
    mergeTrees();
}


void ContourTree::computeContourTree(ScalarGraph& plex) {

    // first compute the augmented contour tree
    computeAugmentedContourTree(plex);

    // now remove the regular vertices
    removeRegularVertices();
    
}


void ContourTree::removeRegularVertices() {
    std::vector<NodeID> nodes = contour_tree.getNodes();    
    std::list<NodeID> regular_nodes;

    for (std::vector<NodeID>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
        if (isRegularNode(*it)) {
            regular_nodes.push_back(*it);
        }
    }

    for (std::list<NodeID>::iterator it=regular_nodes.begin(); it!=regular_nodes.end(); ++it) {

        std::list<NodeID> neighbors = contour_tree.getNeighbors(*it);

        // get the two neighbor node ids
        std::list<NodeID>::iterator neighbor = neighbors.begin();
        NodeID n1 = (*neighbor);
        neighbor++;
        NodeID n2 = (*neighbor);

        // get their members
        std::set<NodeID> m1 = edge_members[Edge(n1,*it)];
        std::set<NodeID> m2 = edge_members[Edge(n2,*it)];

        // union the members
        std::set<NodeID> members;
        members.insert(m1.begin(), m1.end());
        members.insert(m2.begin(), m2.end());

        // add the to-be-deleted middle node to the set
        members.insert(*it);

        // connect the other two nodes
        contour_tree.addEdge(n1, n2);

        // delete the middle node
        contour_tree.removeNode(*it);

        // remove the other edges from the edge_member map
        edge_members.erase(Edge(n1, *it));
        edge_members.erase(Edge(n2, *it));

        // update the new edge's members
        edge_members[Edge(n1, n2)] = members;
    }
}


bool ContourTree::isRegularNode(NodeID u) {
    // Checks to see if a node u in g is regular (does it have exactly one
    // neighbors that is less than it, and one that is greater?
    unsigned int n_lesser = 0;
    unsigned int n_greater = 0;
    double u_height = contour_tree.getValue(u);

    std::list<NodeID> neighbors = contour_tree.getNeighbors(u);
    for (std::list<NodeID>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
        NodeID v = (*it);
        double v_height = contour_tree.getValue(v);
        
        if (v_height > u_height) {
            n_greater++;
        } else {
            n_lesser++;
        }
    }

    return n_greater==1 && n_lesser==1;
}


bool ContourTree::isRooted() {
    return is_rooted;
}


void ContourTree::setRoot(NodeID new_root) {
    // Given a root node ID, changes the directions of the edges
    // in the contour tree so that it is rooted with 'root' as the root.
    
    is_rooted = true;
    root = new_root;

    // compute the predecessor map from a DFS
    std::map<NodeID,NodeID> pm = contour_tree.dfsPredecessorMap(root);

    // now we loop through every node in the graph and direct it towards 
    // its predecessor
    for (std::map<NodeID,NodeID>::iterator it=pm.begin(); it!=pm.end(); ++it) {
        NodeID this_node = (*it).first;
        NodeID predecessor = (*it).second;
        // check to make sure we aren't at the root, then switch the direction
        if (this_node != root)
            contour_tree.addEdge(predecessor, this_node);
    }

    // update the total edge and node weights
    updateTotalWeights(root);
}


NodeID ContourTree::getRoot() {
    if (!is_rooted) throw std::runtime_error("Cannot find the root of an unrooted tree.");
    return root;
}


unsigned int ContourTree::edgeWeight(NodeID x, NodeID y) {
    return edge_members[Edge(x,y)].size();
}


void ContourTree::updateTotalWeights(NodeID u) {
    // if this is a root, the total weight is zero
    total_node_weight[u] = 0;

    if (contour_tree.getNumberOfChildren(u)!=0) {
        // we are going to recursively calculate the weight for
        // each of the children nodes and arcs
        std::list<NodeID> children = contour_tree.getChildren(u);
        for (std::list<NodeID>::iterator child=children.begin(); 
                child!=children.end(); ++child) {

            // update the total weight of this child
            updateTotalWeights(*child);

            // update the total weight of the arc connected to this child
            Edge arc(u,*child);
            total_edge_weight[arc] = edgeWeight(u,*child) + 
                total_node_weight[*child] + 1;

            // add this to the current node's total weight
            total_node_weight[u] += total_edge_weight[arc];
        }
    }
}


unsigned int ContourTree::totalNodeWeight(NodeID id) {
    if (!is_rooted) throw std::runtime_error("Tree is not rooted!");
    return total_node_weight[id];
}


unsigned int ContourTree::totalEdgeWeight(NodeID x, NodeID y) {
    if (!is_rooted) throw std::runtime_error("Tree is not rooted!");
    Edge p(x,y);
    return total_edge_weight[p];
}
