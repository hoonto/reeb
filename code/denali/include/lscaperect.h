#ifndef LSCAPERECT_H_GUARD
#define LSCAPERECT_H_GUARD
#include <iostream> 
#include <vector>
#include "common.h"


////////////////////////////////////////////////////////////////////////////////
//
// class RectangularLandscape
//
////////////////////////////////////////////////////////////////////////////////


class RectangularLandscape {
    struct Contour;
    struct RootContour;
    struct SaddleOuterContour;
    struct SaddleInnerContour;
    struct SaddleLeafContour;
    typedef unsigned long int Index;
public:
    struct Point;
    class Rectangle;
};


////////////////////////////////////////////////////////////////////////////////
//
// struct RectangularLandscape::Point
//
////////////////////////////////////////////////////////////////////////////////


struct RectangularLandscape::Point {
    double x,y,z;
    Point(double x, double y) : x(x), y(y), z(0) {};
    Point(double x, double y, double z) : x(x), y(y), z(z) {};

    friend std::ostream& operator<<(std::ostream& os,
        const RectangularLandscape::Point p);
};


////////////////////////////////////////////////////////////////////////////////
//
// class RectangularLandscape::Rectangle
//
////////////////////////////////////////////////////////////////////////////////


class RectangularLandscape::Rectangle {
    // A class to represent a rectangle.
private:
    Point southwest, northwest, northeast, southeast;
public:
    //
    // Constructors
    //
    Rectangle(double x, double y, double width, double height);

    //
    // Modifying methods
    //
    void scale(double);
    std::vector<RectangularLandscape::Rectangle> split(std::vector<double>);

    //
    // Attributes
    //
    std::vector<RectangularLandscape::Point> points() const;
    double xmin() const;
    double xmax() const;
    double ymin() const;
    double ymax() const;
    double width() const;
    double height() const;
    double area() const;
    double aspectRatio() const;
    Point center() const;

    //
    // Friends
    //
    friend std::ostream& operator<<(std::ostream& os, 
        const RectangularLandscape::Rectangle r);
};


#endif
