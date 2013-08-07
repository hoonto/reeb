import networkx as nx
import numpy as np
import cffi
import re
import os
import os.path


def choose_root(graph, node):
    """
    Given an undirected, acyclic graph, returns the tree induced by taking
    'node' as the root node. Preserves edge and node metadata.
    """
    tree = nx.dfs_tree(graph, node)
    # preserve node metadata
    for u in tree.nodes_iter():
        tree.node[u] = graph.node[u]
    # preserve edge metadata
    for s in tree.edges_iter():
        tree.edge[s[0]][s[1]] = graph.edge[s[0]][s[1]]

    return tree


def uniform_volume(volumeless_tree):
    """
    Equates the edge volume with the number of members it contains.
    """
    tree = volumeless_tree.copy()
    for edge in tree.edges_iter(data=True):
        edge[2]['volume'] = len(edge[2]['members']) + 1
        tree.add_edge(*edge)
    return tree   


def induce_total_volume(volumeless_tree, root=None):
    """
    Given a tree with edges having the 'volume' metadata, computes the
    'total_volume' edge and node property which is the sum of all edge volumes
    between successors. If the 'root' parameter is not supplied, it will be
    determined by performing a topological sort.
    """
    tree = volumeless_tree.copy()
    if root is None:
        root = nx.topological_sort(tree)[0]

    # compute the depth-first search traversal, returning nodes in post-order
    traversal = nx.dfs_postorder_nodes(tree, root)
    for u in traversal:
        tree.node[u]['total_volume'] = 0
        for v in tree.successors(u):
            tree.node[u]['total_volume'] += (tree.node[v]['total_volume'] +
                tree.edge[u][v]['volume'])
            tree.edge[u][v]['total_volume'] = (tree.edge[u][v]['volume'] + 
                tree.node[v]['total_volume'])

    return tree


def pytourtre(neighbors, values):
    """
    Given the neighbors of each node, and the values at each node, returns
    the contour tree as a networkx object.

    Interfaces with the library using CFFI.

    Returns a graph (not a tree!) with each node having the 'height' property
    and each edge having the 'n_members' property.
    """
    ffi = cffi.FFI()

    # define the library functions
    ffi.cdef("void compute_contour_tree( \
            size_t num_vertices, \
            int ** neighbors, \
            int num_neighbors[], \
            size_t * total_order, \
            double * values, \
            int * num_out_nodes, \
            int ** out_nodes, \
            int * num_out_edges, \
            int *** out_edges \
            );")

    # load the library
    libpath = os.path.join(os.path.dirname(os.path.realpath(__file__)), 
        "pytourtre.so")
    lib = ffi.dlopen(libpath)

    # create a neighbors array
    list_of_neighbor_arrays = []
    number_of_neighbors = []
    for x in neighbors:
        list_of_neighbor_arrays.append(ffi.new("int []", x))
        number_of_neighbors.append(len(x))
    arr_neighbors = ffi.new("int * []", list_of_neighbor_arrays)

    # define the output variables as C types
    num_out_nodes = ffi.new("int *")
    num_out_edges = ffi.new("int *")
    out_nodes = ffi.new("int **")
    out_edges = ffi.new("int ***")

    # sort the nodes by value in order to achieve a total ordering
    total_order = sorted(range(len(values)), key=lambda n: values[n])

    # compute the contour tree
    lib.compute_contour_tree(
        len(neighbors),
        arr_neighbors,
        number_of_neighbors,
        total_order,
        values,
        num_out_nodes,
        out_nodes,
        num_out_edges,
        out_edges)

    g = nx.Graph()
    # add all of the nodes along with the 'height' property
    for i in range(num_out_nodes[0]):
        node = out_nodes[0][i]
        g.add_node(node, dict(height=values[node]))

    # and add all of the edges
    for i in range(num_out_edges[0]):
        hi = out_edges[0][i][0]
        lo = out_edges[0][i][1]
        n_members = out_edges[0][i][2]
        g.add_edge(hi,lo,dict(n_members=n_members))

    return g
    

class DisjointSet:
    """
    A disjoint set in the UnionFind family. Supports constant time max and min lookup.
    """
    def __init__(self, initial_object=None):
        self.head = initial_object
        if initial_object is not None:
            self.members = set([initial_object])      
        else:
            self.members = set([])
        
    def __repr__(self):
        return str(self.members)


class UnionFind:
    """
    A family of disjoint sets supporting quick union and find operations.
    """
    def __init__(self):
        self.sets = set([])
        self.set_mapping = {}
    
    def find(self, element):
        """
        Find the representative member of the set in the family which contains
        'element'.
        """
        if element in self.set_mapping:
            return self.set_mapping[element].head
        else:
            new_set = DisjointSet(element)
            self.set_mapping[element] = new_set
            self.sets.add(new_set)
            return element
            
    def union(self, a, b):
        """
        Union the set containing 'a' with the set containing 'b'. The representative
        of 'a' will become the representative of the new set.
        """
        # get the representative members of each set
        rep_a = self.find(a)
        rep_b = self.find(b)
        
        # get the two disjoint sets
        set_a = self.set_mapping[a]
        set_b = self.set_mapping[b]
        
        # union them
        new_set = DisjointSet()
        new_set.members = set_a.members | set_b.members
        new_set.head = set_a.head
        
        # add the new set to the family and update the set mapping
        self.sets.add(new_set)
        for x in set_a.members | set_b.members:
            self.set_mapping[x] = new_set
            
        # delete the old sets
        self.sets.remove(set_a)
        self.sets.remove(set_b)


def join_tree(scalar_graph, total_ordering):
    """
    Given a scalar graph and a total ordering on the nodes, returns the join tree.
    """
    uf = UnionFind()
    tree = nx.DiGraph()
    for u in total_ordering:
        u_rep = uf.find(u)
        for v in scalar_graph.neighbors(u):
            v_rep = uf.find(v)
            if total_ordering.index(v) < total_ordering.index(u) and u_rep != v_rep:
                # make u the parent of v in the join tree
                tree.add_edge(u,v_rep)
                # connect v to u in the union find structure, making the repr of u the 
                # representative of the new set
                uf.union(u,v)
    return tree


def merge_tree(join_tree, split_tree):
    """
    Given a join tree and a split tree, computes a merge tree.
    """
    join_tree = join_tree.copy()
    split_tree = split_tree.copy()
    merge_tree = nx.Graph()
    q = [n for n in join_tree if join_tree.out_degree(n) + split_tree.out_degree(n) == 1]
    while len(q) >= 2:
        i = q.pop()
        if join_tree.out_degree(i) == 0:
            k = join_tree.predecessors(i)[0]
            merge_tree.add_edge(i,k)
            join_tree.remove_node(i)
            if split_tree.in_degree(i) == 0:
                split_tree.remove_node(i)
            else:
                parent = split_tree.predecessors(i)[0]
                child = split_tree.successors(i)[0]
                split_tree.add_edge(parent, child)
                split_tree.remove_node(i)
            if join_tree.out_degree(k) + split_tree.out_degree(k) == 1:
                q.append(k)
        else:
            k = split_tree.predecessors(i)[0]
            merge_tree.add_edge(i,k)
            split_tree.remove_node(i)
            if join_tree.in_degree(i) == 0:
                join_tree.remove_node(i)
            else:
                parent = join_tree.predecessors(i)[0]
                child = join_tree.successors(i)[0]
                join_tree.add_edge(parent, child)
                join_tree.remove_node(i)
            if join_tree.out_degree(k) + split_tree.out_degree(k) == 1:
                q.append(k)
    return merge_tree


def augmented_contour_tree(scalar_graph):
    """
    Computes the augmented contour tree of a scalar graph, where each node has the 'height' attribute.
    """
    total_ordering = sorted(scalar_graph.nodes(), key=lambda n: scalar_graph.node[n]['height'])
    jt = join_tree(scalar_graph, total_ordering)
    st = join_tree(scalar_graph, total_ordering[::-1])
    mt = merge_tree(jt, st)
    # copy node attributes
    for node in scalar_graph.nodes_iter():
        mt.node[node] = scalar_graph.node[node]
    return mt


def remove_regular_vertices(augmented_contour_tree):
    """
    Given an augmented contour tree 'act', removes the regular vertices, producing the contour tree.
    """
    tree = augmented_contour_tree.copy()
    # give each edge in the tree the "members" property
    for u,v in tree.edges_iter():
        tree.edge[u][v]['members'] = set([])
    for u in tree.nodes():
        if is_regular(tree, u):
            x,y = tree.neighbors(u)
            members = tree.edge[x][u]["members"] | tree.edge[u][y]["members"] | set([u])
            tree.add_edge(x,y,dict(members=members))
            tree.remove_node(u)
    return tree
        

def is_regular(tree, u):
    """
    Checks to see if a node is regular (does it have exactly one neighbor that
    is less than it, and one that is greater?).
    """
    n_lesser = 0
    n_greater = 0
    u_height = tree.node[u]['height']
    for v in tree[u]:
        v_height = tree.node[v]['height']
        if v_height > u_height:
            n_greater += 1
        elif v_height < u_height:
            n_lesser += 1
    return n_greater == 1 and n_lesser == 1


def contour_tree(scalar_graph):
    """
    Computes the contour tree from a scalar graph.
    """
    act = augmented_contour_tree(scalar_graph)
    ct = remove_regular_vertices(act)
    return ct
