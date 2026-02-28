#include "PPP/Simple_window.h" // get access to our window library
#include "PPP/Graph.h"         // get access to our graphics library facilities

double dsin(double d) { return sin(d); }

int main()
{
    using namespace Graph_lib; // our graphics facilities are in Graph_lib

    Application app; // start a Graphics/GUI application

    Point tl{900, 500}; // to become top left corner of window

    Simple_window win{tl, 600, 400, "Canvas"}; // make a simple window

    Polygon poly;               // make a shape (a polygon)
    poly.add(Point{300, 200});  // add a point
    poly.add(Point{350, 100});  // add another point
    poly.add(Point{400, 200});  // add a third point
    poly.set_color(Color::red); // adjust properties of poly
    poly.set_style(Line_style(Line_style::dash, 4));
    win.attach(poly); // connect poly to the window

    // 10.7.1
    Axis xa{Axis::x, Point{20, 300}, 280, 10, "x axis"};
    win.attach(xa);

    Axis ya{Axis::y, Point{20, 300}, 280, 10, "y axis"};
    ya.set_color(Color::cyan);
    ya.label.set_color(Color::dark_red);
    win.attach(ya);

    // 10.7.2
    Function sine{dsin, 0, 100, Point{20, 150}, 1000, 50, 50};
    win.attach(sine);

    // 10.7.4
    Rectangle r{Point{200, 200}, 100, 50};
    r.set_fill_color(Color::yellow);
    win.attach(r);

    Closed_polyline poly_rect;
    poly_rect.add(Point{100, 50});
    poly_rect.add(Point{200, 50});
    poly_rect.add(Point{200, 100});
    poly_rect.add(Point{100, 100});
    poly_rect.add(Point{50, 75});
    poly_rect.set_style(Line_style(Line_style::dash, 2));
    poly_rect.set_fill_color(Color::green);
    win.attach(poly_rect);

    // 10.7.5
    Text t{Point{150, 150}, "Hello, graphical world!"};
    t.set_font(Font::times_bold);
    t.set_font_size(20);
    win.attach(t);

    Image copter{Point{100, 50}, "mars_copter.jpg"};
    copter.move(100, 250);
    win.attach(copter);

    Circle c{Point{100, 200}, 50};

    Ellipse e{Point{100, 200}, 75, 25};
    e.set_color(Color::dark_red);

    Mark m{Point{100, 200}, 'x'};
    m.set_color(Color::red);
    ostringstream oss;
    oss << "screen size: " << x_max() << "*" << y_max()
        << "; window size: " << win.x_max() << "*" << win.y_max();
    Text sizes{Point{100, 20}, oss.str()};

    Image scan{Point{275, 225}, "scandinavia.jfif"};
    scan.scale(150, 200);

    win.attach(c);
    win.attach(m);
    win.attach(e);

    win.attach(sizes);
    win.attach(scan);

    win.set_label("Test GUI");
    win.wait_for_button(); // give control to the display engine
}
