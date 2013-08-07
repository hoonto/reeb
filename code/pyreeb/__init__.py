import numpy as np
import networkx as nx

class Rectangle():
    def __init__(self, x_min=0, x_max=1, y_min=0, y_max=1):
        self._bounds = [x_min, x_max, y_min, y_max]
        
    def __repr__(self):
        return "Rectangle(x_min={0}, x_max={1}, y_min={2}, y_max={3})".format(*self._bounds)
        
    def points(self):
        """
        Returns the corner points of the rectangle as a list.
        The list's order is set such that the 0th point is the lower left, and the
        indices increase clockwise.
        """
        xmin, xmax, ymin, ymax = self._bounds
        points = []
        points.append([xmin,ymin])
        points.append([xmin,ymax])
        points.append([xmax,ymax])
        points.append([xmax,ymin])
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
    
    def split(self, pct):
        """
        Splits the rectangle in two, such that the left rectangle has pct% of the total area.
        """
        xmin, xmax, ymin, ymax = self._bounds
        split_point = self.width()*float(pct) + xmin
        left_rectangle = Rectangle(xmin, split_point, ymin, ymax)
        right_rectangle = Rectangle(split_point, xmax, ymin, ymax)
        return left_rectangle, right_rectangle


def embed_contour_tree(tree, root_node, bounding_rectangle=None):
    """
    Given a tree and a root node, draws the landscape metaphor. The landscape
    is contained within the domain specified by the bounding_rectangle.
    """
    landscape_tree = tree.copy()
    if bounding_rectangle is None:
        bounding_rectangle = Rectangle(x_min=0,x_max=32,y_min=0,y_max=32)

    # embed the root contour
    landscape_tree.node[root_node]['rectangle'] = bounding_rectangle

    # there should be a single child
    successors = tree.successors(root_node)
    assert len(successors)==1, "Root node is not degree 1."

    # the successor should be degree 3
    assert len(tree.successors(successors[0]))>=2, "Root node does not point to node of degree at least 3."
    embed_contour_subtree(landscape_tree, successors[0], bounding_rectangle)
    return landscape_tree      

