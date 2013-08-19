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


// SaddleOuterContour
////////////////////////////////////////////////////////////////////////////////


struct RectangularLandscape::SaddleOuterContour : public Contour {
    Rectangle rectangle;
    std::vector<SaddleInnerContour> children;

    SaddleOuterContour(NodeID node, Rectangle rectangle, double altitude)
            : Contour(node, altitude), rectangle(rectangle) { }

    std::vector<Point> enumerateIndices(Index);
};


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

