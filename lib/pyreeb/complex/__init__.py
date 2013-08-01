import networkx as nx
import sklearn.neighbors
import numpy as np

def _lower_neighbors(g, u):
    return filter(lambda v : v<u, nx.neighbors(g,u))


def _inductive_vr(g,k):
    K = []
    for node in g.nodes():
        K.append((node,))
    K.extend(g.edges())
    for i in range(1,k):
        # find the i-simplices in the graph
        for simplex in [s for s in K if len(s)-1==i]:
            # find the intersection of the lower neighbors
            # of every point in the simplex
            N = set(g.nodes())
            for u in simplex:
                N = N & set(_lower_neighbors(g, u))
            for v in N:
                K.append(tuple(list(simplex) + [v]))
                
    return K


def vietoris_rips(data, radius):
    """
    Builds a Rips complex from the data, given the radius.
    """
    # compute the neighbors graph
    nn = sklearn.neighbors.radius_neighbors_graph(data, radius)
    # construct a proximity graph from this adjacency matrix
    full_graph = nx.Graph(nn)
    g = nx.connected_component_subgraphs(full_graph)[0]
    # remove self-loop-edges
    g.remove_edges_from(g.selfloop_edges())
    
    return _inductive_vr(g, 2)


def kneighbors(data, k):
    """
    Builds a complex from the data by connecting each point to the k nearest
    neighbors.
    """
    nn = sklearn.neighbors.kneighbors_graph(data, k)
    # get an array of u indices and v indices
    u,v = np.nonzero(nn)
    # and zip them together
    edges = zip(u.tolist(), v.tolist())
    # make a graph complex and return it
    g = nx.Graph()
    g.add_edges_from(edges)
    g.remove_edges_from(g.selfloop_edges())
    return g
