import networkx as nx
import numpy as np

class Rectangle():
    """
    A class to represent a rectangle.
    """
    def __init__(self, x_min=0, x_max=1, y_min=0, y_max=1):
        self._bounds = [x_min, x_max, y_min, y_max]
        
    def __repr__(self):
        return "Rectangle(x_min={0}, x_max={1}, y_min={2}, y_max={3})".format(*self._bounds)
        
    def points(self):
        """
        Returns the corner points of the rectangle as a list.
        The list's order is set so that the points are [southwest, northwest, southeast, northeast]
        """
        xmin, xmax, ymin, ymax = self._bounds
        points = []
        points.append([xmin,ymin])
        points.append([xmin,ymax])
        points.append([xmax,ymin])
        points.append([xmax,ymax])
        return points
    
    def scale(self, final_area):
        """
        Scales the rectangle to area final_area without translation.
        """
        xmin, xmax, ymin, ymax = self._bounds
        aspect_ratio = self.aspect_ratio()
        new_width = np.sqrt(final_area/aspect_ratio)
        new_height = aspect_ratio*new_width
        x_delta = (new_width - (xmax - xmin))/2
        y_delta = (new_width - (ymax - ymin))/2
        self._bounds = [xmin-x_delta, xmax+x_delta, ymin-y_delta, ymax+y_delta]
        
    def copy(self):
        return Rectangle(*self._bounds)
    
    def bounds(self):
        return list(self._bounds)
        
    def width(self):
        return self._bounds[1] - self._bounds[0]
    
    def height(self):
        return self._bounds[3] - self._bounds[2]
    
    def area(self):
        return self.height()*self.width()
    
    def center(self):
        xmin, xmax, ymin, ymax = self._bounds
        return (xmax+xmin)/2.0, (ymax+ymin)/2.0
    
    def aspect_ratio(self):
        return self.height()/float(self.width())
    
    def split(self, shares):
        """
        Splits the rectangle in two, such that the left rectangle has pct% of the total area.
        """
        x_min, x_max, y_min, y_max = self._bounds
        
        cursor = x_min
        rectangles = []
        assert sum(shares) <= 1, "Shares must sum to at most one."
        for s in shares:
            delta = self.width()*s
            rectangles.append(Rectangle(cursor, cursor+delta, y_min, y_max))
            cursor += delta
            
        return rectangles


class Contour:
    """
    A class to represent a rectangular contour.
    """
    def __init__(self):
        self.type = None
        self.indices = None
        
    def __repr__(self):
        return self.type


class RootContour(Contour):
    def __init__(self, rectangle, height):
        self.type = "root"
        self.rectangle = rectangle
        self.indices = None
        self.height = height
        
    def enumerate_indices(self, base):
        self.indices = (base + np.arange(4)).tolist()
        points = self.rectangle.points()
        points = np.column_stack((points, np.repeat(self.height, len(points))))
        return points.tolist()


class SaddleOuterContour(Contour):
    def __init__(self, rectangle, height, children=None):
        self.type = "saddleouter"
        self.rectangle = rectangle
        self.indices = None
        self.height = height
        if children is None:
            self.children = []
        else:
            self.children = children
            
    def enumerate_indices(self, base):
        points = []
        self.indices = [base, base+1]
        
        for inner_contour in self.children:
            inner_rectangle = inner_contour.rectangle
            points.extend(inner_rectangle.points()[:2])
            inner_indices = list([base, base+1, base+2, base+3])
            inner_contour.indices = inner_indices
            base = base+2
            
        points.extend(inner_rectangle.points()[2:])
        points = np.column_stack((points, np.repeat(self.height, len(points))))
            
        self.indices.extend([base, base+1])
        return points.tolist()  


class SaddleInnerContour(Contour):
    def __init__(self, rectangle, child):
        self.type = "saddleinner"
        self.rectangle = rectangle
        self.child = child
        self.indices = None


class LeafContour(Contour):
    def __init__(self,*args):
        self.type = "leaf"
        self.x, self.y, self.height = args
        self.indices = None

    def enumerate_indices(self, base):
        self.indices = [base]
        return [[self.x,self.y,self.height]]


def map_nodes_to_contours(tree, bounding_rectangle=None):
    """
    Given a tree, returns a tree where each node is mapped to a contour object.
    """
    if bounding_rectangle is None:
        bounding_rectangle = Rectangle(0,10,0,10)
    landscape = tree.copy()
    
    # find the root
    root_node = [n for n in landscape if landscape.in_degree(n)==0][0]
    # handle the root node
    root_height = landscape.node[root_node]['height']
    root_contour = RootContour(bounding_rectangle, root_height)
    landscape.node[root_node]['contour'] = root_contour
    
    # recurse (the child should be a saddle)
    child_node = landscape.successors(root_node)[0]
    map_saddle_to_contour(landscape, child_node, root_contour)
            
    return landscape


def map_leaf_to_contour(landscape, node, parent_contour):
    """
    Map a leaf node to its contour.
    """
    x,y = parent_contour.rectangle.center()
    height = landscape.node[node]['height']
    leaf_contour = LeafContour(x,y,height)
    landscape.node[node]['contour'] = leaf_contour


def map_saddle_to_contour(landscape, node, parent_contour):
    """
    Map a saddle to its contour.
    """
    # calculate the correct area of the outer rectangle
    parent = landscape.predecessors(node)[0]
    parent_area = parent_contour.rectangle.area()
    parent_volume = landscape.node[parent]['total_volume']
    outer_volume = landscape.node[node]['total_volume']
    outer_area = float(outer_volume)/parent_volume * parent_area
    
    # now scale the outer rectangle down to the appropriate area
    outer_rectangle = parent_contour.rectangle.copy()
    outer_rectangle.scale(outer_area)
    
    # make the outer contour
    outer_height = landscape.node[node]['height']
    outer_contour = SaddleOuterContour(outer_rectangle, outer_height)    
    
    # split the outer rectangle into pieces for the children
    inner_volumes = [landscape.edge[node][n]['total_volume'] for n in landscape[node]]
    inner_shares = [float(v)/outer_volume for v in inner_volumes]
    inner_rectangles = outer_rectangle.split(inner_shares)
    
    children = landscape.successors(node)
    for child, inner_rectangle in zip(children, inner_rectangles):
        inner_contour = SaddleInnerContour(inner_rectangle, child)
        outer_contour.children.append(inner_contour)
        
        if landscape.out_degree(child) == 0:
            map_leaf_to_contour(landscape, child, inner_contour)
        else:
            map_saddle_to_contour(landscape, child, inner_contour)
            
    landscape.node[node]['contour'] = outer_contour


def enumerate_contour_points(landscape):
    """
    Enumerate the points in the landscape, returning an array of the points and
    mapping each contour in the tree to this list.
    """
    points = []
    base = 0
    for node in landscape:
        node_points = landscape.node[node]['contour'].enumerate_indices(base)
        base += len(node_points)
        points.extend(node_points)
    return np.array(points)


def triangulate_rectangular_contours(landscape):
    """
    Given an enumerated contour landscape tree, returns a list of the triangles in the landscape metaphor.
    """
    root_node = [n for n in landscape if landscape.in_degree(n)==0][0]
    triangles = []
    
    # handle the root node
    child_node = landscape.successors(root_node)[0]
    outer_inds = landscape.node[root_node]['contour'].indices
    inner_inds = landscape.node[child_node]['contour'].indices
    triangles.extend(triangulate_nested_rectangle(outer_inds, inner_inds))
    
    # now work through the remaining nodes
    dfs_nodes = nx.dfs_preorder_nodes(landscape, root_node)
    dfs_nodes.next()
    for node in dfs_nodes:
        # we only do something if this is not a leaf
        is_leaf = landscape.out_degree(node)==0
        if not is_leaf:
            outer_saddle_contour = landscape.node[node]['contour']
            for inner_saddle_contour in outer_saddle_contour.children:
                child = inner_saddle_contour.child
                child_contour = landscape.node[child]['contour']
                inner_saddle_indices = inner_saddle_contour.indices
                child_indices = child_contour.indices
                
                if child_contour.type == 'leaf':
                    triangles.extend(triangulate_nested_point(inner_saddle_indices, child_indices[0]))
                else:
                    triangles.extend(triangulate_nested_rectangle(inner_saddle_indices, child_indices))
        
    return triangles
    return np.array(points)


def triangulate_nested_rectangle(outer, inner):
    """
    Given the indices of the corner points of an outer rectangle in conventional order, and the 
    indices of the corner points of an inner rectangle in conventional order, returns a list of
    3-tuples of indices which represent the triangulation of the area in between the rectangles.
    """
    triangles = []
    triangles.append([outer[0], inner[0], inner[1]])
    triangles.append([outer[1], inner[1], inner[3]])
    triangles.append([outer[3], inner[3], inner[2]])
    triangles.append([outer[2], inner[0], inner[2]])
    triangles.append([outer[0], outer[1], inner[1]])
    triangles.append([outer[1], outer[3], inner[3]])
    triangles.append([outer[3], outer[2], inner[2]])
    triangles.append([outer[2], outer[0], inner[0]])
    return triangles


def triangulate_nested_point(rectangle, point):
    """
    Given the indices of the corner points of an outer rectangle in conventional order, and
    the index of a point that lies inside the rectangle, returns a list of 3-tuples of
    indices which represent the triangulation of the point within the rectangle.
    """
    triangles = []
    triangles.append([rectangle[0], rectangle[1], point])
    triangles.append([rectangle[1], rectangle[3], point])
    triangles.append([rectangle[3], rectangle[2], point])
    triangles.append([rectangle[2], rectangle[0], point])
    return triangles


def scale_point_heights(points, z_range):
    """
    Scales the z coordinate of points to fall within z_range.
    """
    z = points[:,2]
    alpha = float(z_range[1] - z_range[0])/(np.max(z) - np.min(z))
    beta = alpha*np.min(z) - z_range[0]
    z = alpha*z + beta
    scaled_points = points.copy()
    scaled_points[:,2] = z
    return scaled_points
