import re

def read_OFF(filename):
    """
    Given the path of an OFF file, reads in the file, returning a list of the
    vertex positions and a list of each simplex in the file. 

    This is useful to read the result of RandReeb. Since RandReeb takes as
    input an OFF file where the vertices are embedded in either R^2 or R^3,
    and we are generally dealing with high-dimensional data, the vertex list
    is generally going to be useless. The list of simplices, however, will
    be an edge list. We can then represent the graph easily using networkx:
        
        vertices, simplices = read_OFF(filename)
        g = networkx.Graph()
        g.add_edges_from(simplices)

    """
    with open(filename) as f:
        # read until we hit the line describing the number of vertices
        while True:
            line = f.readline().strip()
            match = re.match("^(\d+) (\d+) (\d+)$", line)
            if match:
                n_vertices = int(match.groups()[0])
                n_simplices = int(match.groups()[1])
                break

        # read in the vertices
        vertices = []
        for i in range(n_vertices):
            v = [float(x) for x in f.readline().strip().split()]
            vertices.append(v)

        # read in the simplices
        simplices = []
        for i in range(n_simplices):
            line = f.readline().strip().split()
            dim = int(line[0])
            s = [int(x) for x in line[1:dim+1]]
            simplices.append(s)

    return vertices, simplices
