#include <cmath>
#include "lscaperect.h"

namespace{
void broadcastAltitude(std::vector<RectangularLandscape::Point>&, double);
}

////////////////////////////////////////////////////////////////////////////////
//
// RectangularLandscape::Rectangle Methods
//
////////////////////////////////////////////////////////////////////////////////


RectangularLandscape::Rectangle::Rectangle(
        double x,
        double y,
        double width,
        double height)
        : southwest(x-width/2,y-height/2), southeast(x+width/2,y-height/2),
          northwest(x-width/2,y+height/2), northeast(x+width/2,y+height/2) { }


void RectangularLandscape::Rectangle::scale(double final_area) {
    typedef RectangularLandscape::Point Point;
    double new_width = std::sqrt(final_area/aspectRatio());
    double new_height = aspectRatio() * new_width;
    
    (*this) = RectangularLandscape::Rectangle(center().x, center().y, 
        new_width, new_height);
}


std::vector<RectangularLandscape::Rectangle> 
RectangularLandscape::Rectangle::split(std::vector<double> shares) {
    // keep track of the rightmost head of the rectangle
    double head = xmin();

    // and store the resulting rectangles
    std::vector<RectangularLandscape::Rectangle> rectangles;

    for (std::vector<double>::iterator it=shares.begin(); it!=shares.end(); ++it) {
        double share_width = width() * (*it);
        double new_x = head + share_width/2;
        rectangles.push_back(Rectangle(new_x, center().y, share_width, height()));
        head += share_width;
    }
    
    return rectangles;
}


std::vector<RectangularLandscape::Point> 
RectangularLandscape::Rectangle::points() const {
    std::vector<Point> pts;
    pts.push_back(southwest);
    pts.push_back(northwest);
    pts.push_back(northeast);
    pts.push_back(southeast);
    return pts;
}


double RectangularLandscape::Rectangle::xmin() const {
    return southwest.x;
}


double RectangularLandscape::Rectangle::ymin() const {
    return southwest.y;
}


double RectangularLandscape::Rectangle::xmax() const {
    return northeast.x;
}


double RectangularLandscape::Rectangle::ymax() const {
    return northeast.y;
}


double RectangularLandscape::Rectangle::width() const {
    return xmax() - xmin();
}


double RectangularLandscape::Rectangle::height() const {
    return ymax() - ymin();
}


double RectangularLandscape::Rectangle::area() const {
    return width() * height();
}


double RectangularLandscape::Rectangle::aspectRatio() const {
    return height() / width();
}


RectangularLandscape::Point RectangularLandscape::Rectangle::center() const {
    double x, y;
    x = (xmax() + xmin())/2;
    y = (ymax() + ymin())/2;
    return RectangularLandscape::Point(x,y);
}


std::ostream& operator<<(std::ostream& os, 
        const RectangularLandscape::Rectangle r) {
    os << "Rectangle(x=" << r.center().x << ", y=" << r.center().y;
    os << ", width=" << r.width() << ", height=" << r.height() << ")";
}


////////////////////////////////////////////////////////////////////////////////
//
// RectangularLandscape::Point Methods
//
////////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream& os, 
        const RectangularLandscape::Point p) {
    os << "Point(" << p.x << ", " << p.y << ")";
}


////////////////////////////////////////////////////////////////////////////////
//
// Contour Classes
//
////////////////////////////////////////////////////////////////////////////////


// Contour
////////////////////////////////////////////////////////////////////////////////


struct RectangularLandscape::Contour {
    std::vector<Index> indices;
    NodeID node;
    double altitude;
    Contour(NodeID node, double altitude) : node(node), altitude(altitude) { }
};


// RootContour
////////////////////////////////////////////////////////////////////////////////


struct RectangularLandscape::RootContour : public Contour {
    Rectangle rectangle;

    RootContour(NodeID node, Rectangle rectangle, double altitude)
            : Contour(node, altitude), rectangle(rectangle) { }
        
    std::vector<Point> enumerateIndices(Index);
};


std::vector<RectangularLandscape::Point> 
RectangularLandscape::RootContour::enumerateIndices(Index base) {
    // update the indices
    indices.clear();
    indices.push_back(base);
    indices.push_back(base+1);
    indices.push_back(base+2);
    indices.push_back(base+3);

    std::vector<Point> points = rectangle.points();
    broadcastAltitude(points, altitude);

    return points;
}


// SaddleInnerContour
////////////////////////////////////////////////////////////////////////////////
struct RectangularLandscape::SaddleInnerContour : public Contour {
    Rectangle rectangle;

    SaddleInnerContour(NodeID node, Rectangle rectangle, double altitude) 
            : Contour(node, altitude), rectangle(rectangle) { }
};


// SaddleOuterContour
////////////////////////////////////////////////////////////////////////////////


struct RectangularLandscape::SaddleOuterContour : public Contour {
    Rectangle rectangle;
    std::vector<SaddleInnerContour> children;

    SaddleOuterContour(NodeID node, Rectangle rectangle, double altitude)
            : Contour(node, altitude), rectangle(rectangle) { }

    std::vector<Point> enumerateIndices(Index);
};


std::vector<RectangularLandscape::Point>
RectangularLandscape::SaddleOuterContour::enumerateIndices(Index base) {
    // We will add the points in a counter-clockwise order. First, we enumerate
    // the points from the left side of the bounding rectangle.
    std::vector<Point> points;
    std::vector<Point> outer_rectangle = rectangle.points();
    points.insert(points.end(), outer_rectangle.begin(), 
        outer_rectangle.begin()+2);
    indices.push_back(base);
    indices.push_back(base+1);

    typedef std::vector<SaddleInnerContour> Children;
    // INVARIANTS:
    // base: the index of the lower left point of the inner_rectangle
    for (Children::iterator it=children.begin(); it!=children.end(); ++it) {
        // alias the child
        SaddleInnerContour& child = (*it);

        // get the inner rectangle and the inner rectangle's points
        Rectangle inner_rectangle = child.rectangle;
        std::vector<Point> inner_points = inner_rectangle.points();

        // rely on the fact that the points are returned in a specific order to
        // insert them into the outer contour's points list, but only add the 
        // points from the right side of the inner rectangle, as the ones on
        // the left hand side were added by the previous iteration of the loop
        points.insert(points.end(), inner_points.begin()+2, inner_points.end());

        // calculate the indices of the inner points.
        child.indices.clear();
        child.indices.push_back(base);
        child.indices.push_back(base+1);
        child.indices.push_back(base+2);
        child.indices.push_back(base+3);

        // maintain the loop invariant. Since there are two new points (the 
        // other two were seen in the previous iteration), we increment base
        // by exactly two.
        base += 2;
    }
    
    // update the heights
    broadcastAltitude(points, altitude);

    return points;
}


////////////////////////////////////////////////////////////////////////////////
//
// Helper Functions
//
////////////////////////////////////////////////////////////////////////////////


namespace{


void broadcastAltitude(std::vector<RectangularLandscape::Point>& points, 
        double altitude) {
    typedef std::vector<RectangularLandscape::Point> Points;
    
    for (Points::iterator it=points.begin(); it!=points.end(); ++it) {
        (*it).z = altitude;
    }
}


} // namespace 

