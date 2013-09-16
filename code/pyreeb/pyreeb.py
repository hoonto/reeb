import numpy as np
import sklearn.neighbors
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.patches
import scipy
import re
import subprocess

import pdb


###############################################################################
#
# SIMPLICIAL COMPLEXES
#
###############################################################################


def triangle_cliques(connections):
    """
    Returns all 3-cliques implied by the upper-triangular sparse connections 
    matrix.
    """
    triangles = []
    for i in range(connections.shape[0]):
        adjacency = connections.getrow(i)
        # ignore self loops
        adjacency[0,i] = False
        neighbors = np.nonzero(adjacency)[1].tolist()
        while neighbors:
            j = neighbors.pop(0)
            for k in neighbors:
                if connections[j,k]:
                    triangles.append([i,j,k])

    return triangles

         
def kneighbors_2skeleton(data, k):
    """
    Builds the 2-skeleton of the data by connecting each point to its k nearest
    neighbors, then making triangles out of sets of 3 mutually-connected
    vertices. Returns a list of edges and a list of triangles.
    """
    # compute the k nearest neighbors
    knn = sklearn.neighbors.kneighbors_graph(data, k)
    # make the connections symmetric, and return only the upper triangle
    connections = scipy.sparse.triu((knn + knn.T).astype(bool)).tocsr()

    # find mutually-connected triangles
    triangles = triangle_cliques(connections)

    # get the list of edges
    u,v = np.nonzero(connections)
    edges = zip(u.tolist(), v.tolist())
    # remove self edges
    edges = [e for e in edges if e[0] != e[1]]

    return edges, triangles


def plot_2d_complex(data, edges, tris):
    """
    Plots a complex whose points are embedded in 2d Euclidean space.
    """
    # draw the points
    plt.scatter(data[:,0], data[:,1])
    # now draw each edge
    for u,v in edges:
        x = data[(u,v),0]
        y = data[(u,v),1]
        plt.plot(x, y, color='black', alpha=0.5)

    for u,v,w in tris:
        points = data[(u,v,w),:]
        poly = matplotlib.patches.Polygon(points, facecolor='red', alpha=0.2)
        plt.gca().add_patch(poly)

    plt.show()
        

###############################################################################
#
# GRAPH FUNCTIONS
#
###############################################################################


def geodesic_distance(plex, root=None):
    """
    Given a simplicial complex in the form of a networkx graph, computes the
    geodesic distance to each node from a given root node. If the root is not
    specified, an arbitrary extreme root is found by eccentricity. Returns a
    list of distances.


    Assumes the graph is connected.
    """
    if root is None:
        # compute the eccentricity of each node
        max_distances = nx.eccentricity(plex)
        root = max(max_distances.items(), key=lambda x: x[1])[0]

    # compute the distance from the root to each node
    distances = nx.shortest_path_length(plex, root)
    return [distances[v] for v in sorted(plex.nodes())]


###############################################################################
#
# RANDREEB INTERFACE
#
###############################################################################

def randreeb(heights, edges, tris):
    """
    Interfaces with the randreeb program to compute the reeb graph of a 
    function defined on the 2-skeleton of a simplicial complex.
    """
    # write the complex
    write_OFF("complex.off", heights, edges, tris)
    # call randreeb
    output = subprocess.check_output(["./randreeb/ReebGraph", 
            "-x", "-c", "complex.off"])
    # read the output
    values, edges = read_OFF("out_aug.off")
    return edges


###############################################################################
#
# OFF FILE HANDLING
#
###############################################################################


def read_OFF(filepath):
    """
    Reads an OFF file, returning a list of values on the vertices, and a list
    of faces (edges are ignored).
    """
    with open(filepath,"r") as f:
        # read until we get to the first line containing the counts
        line = f.readline()
        pattern = "^(\d+) (\d+) (\d+)"
        matches = re.findall(pattern, line)
        while not matches:
            line = f.readline()
            matches = re.findall(pattern, line)
        n_vertices, n_faces, _ = [int(x) for x in matches[0]]

        # read the vertices in, taking only the last number of each line
        values = []
        for i in range(n_vertices):
            line = f.readline().strip()
            vertex = [float(x) for x in line.split()]
            values.append(vertex[-1])

        # read the faces in
        faces = []
        for i in range(n_faces):
            line = f.readline().strip()
            face = [int(x) for x in line.split()]
            dim = face[0]
            faces.append(face[1:dim+1])

        return values, faces


def write_OFF(filepath, values, edges, faces):
    """
    Writes an OFF file that is readable by the above.
    """
    with open(filepath,"w+") as f:
        f.write("OFF\n")
        f.write("{} {} 0\n".format(len(values), len(edges)+len(faces)))
        for v in values:
            f.write("{}\n".format(v))
        
        for x in edges+faces:
            f.write("{} ".format(len(x)))
            f.write(" ".join([str(y) for y in x]))
            f.write("\n")
