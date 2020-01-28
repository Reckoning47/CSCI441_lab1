// Chris Cooper
// CSCI 441
// Lab 1

#include <iostream>
#include <string>
#include <cmath>
#include "bitmap_image.hpp"

//namespace
using namespace std;

//Class for storing both position and color information for each Vertex of the triangle
class Vertex{
public:
    float x;
    float y;
    float r;
    float g;
    float b;

    // Constructor
    Vertex(float xx, float yy, float rr, float gg, float bb) : x(xx), y(yy), r(rr), g(gg), b(bb) {
        //cout << "Construct Vertex" << endl;
    }

    // Default constructor
    Vertex() : x(0), y(0), r(0), g(0), b(0) {
        //cout << "Construct Default Vertex" << endl;
    }

    ~Vertex() {
        // cout << "in Vertex destructor" << endl;
    }

    void SetVertex(float xx, float yy, float rr, float gg, float bb) {
        x = xx;
        y = yy;
        r = rr;
        g = gg;
        b = bb;
    }
};

//2D Vector for cartesian coordinates
class Vector2 {
public:
    float x;
    float y;

    //Constructors
    Vector2(float xx, float yy) : x(xx), y(yy) {}
    Vector2() : x(0), y(0) {}
    ~Vector2() {
        // this is where you would release resources such as memory or file descriptors
        // td::cout << "in Vector2 destructor" << endl;
    }

};

//3D Vector for colors and barycentric coordinates
class Vector3 {
public:
    float x;
    float y;
    float z;

    // Parameterized constructor
    Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
    // default constructor sets all to 0
    Vector3() : x(0), y(0), z(0) {}

    void Add(Vector3 v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void Scale(float a) {
        x *= a;
        y *= a;
        z *= a;
    }
};

// Helper class for creating lines
class Line {
public:
    float ly;
    float lx;
    float lz;

    //Constructor
    Line(Vector2 p, Vector2 q) {
        ly = (q.y - p.y);
        lx = (p.x - q.x);
        lz = (ly*p.x) + (lx*p.y);
    }

    float Val(Vector2 p) {
        return (ly*p.x) + (lx*p.y) - lz;
    }
};

// Helper class to create box from 2D vectors
class Box {
public:
    Vector2 vec_1;
    Vector2 vec_2;
    // Constructor
    Box(Vector2* vectors) {
        vec_1.x = vectors[0].x;
        vec_2.x = vectors[0].x;
        vec_1.y = vectors[0].y;
        vec_2.y = vectors[0].y;

        // loop through and set x and y coordinates for the line
        for (int i = 1; i < 3; i++) {
            if (vectors[i].x > vec_2.x) {
                vec_2.x = vectors[i].x;
            }
            if (vectors[i].y < vec_1.y) {
                vec_1.y = vectors[i].y;
            } else if (vectors[i].y > vec_2.y) {
                vec_2.y = vectors[i].y;
            }
        }
    }
};

// Converts float to int for color usage using bitmap_image round method
int foi(float input) {
    return round(input * 255);
}

// Overrides stream output for data vertices
ostream& operator<<(ostream& stream, const Vertex& p) {
    stream << "Vertex: (" << p.x << "," << p.y << ") Color: (" << p.r << "," << p.g << "," << p.b << ")";
    return stream;
}

// return position in vertex form
Vector2 GetPosition(Vertex p) {
    Vector2 position(p.x,p.y);
    return position;
}

// return color in vertex form
Vector3 GetColor(Vertex p) {
    Vector3 color(p.r,p.g,p.b);
    return color;
}

// Calculate beta, gamma, and alpha from class formulas
Vector3 BaryCoord(Line ab, Line ac, Vector2 b, Vector2 c, Vector2 p) {
    float beta = ac.Val(p) / ac.Val(b);
    float gamma = ab.Val(p) / ab.Val(c);
    float alpha = 1 - beta - gamma;
    return Vector3(beta, gamma, alpha);
}

// Checks if the barycentric coordinates are inside the triangle
bool isBarycentric(Vector3 v) {
    if (v.x < 0 || v.x > 1 || v.y < 0 || v.y > 1 || v.z < 0 || v.z > 1) {
        return false;
    }
    return true;
}

//Finds the color for the given barycentric coordinate given the points in the triangle
Vector3 GetColorBarycentric(Vertex* Vertices, Vector3 coord) {
    // create new Vector3 object and return the colors of the provided vertices
    // using the value of their coordinates
    Vector3 new_color_vec = Vector3();
    Vector3 color_vec = GetColor(Vertices[0]);
    color_vec.Scale(coord.x);
    new_color_vec.Add(color_vec);
    color_vec = GetColor(Vertices[1]);
    color_vec.Scale(coord.y);
    new_color_vec.Add(color_vec);
    color_vec = GetColor(Vertices[2]);
    color_vec.Scale(coord.z);
    new_color_vec.Add(color_vec);
    return new_color_vec;
}

// Creates the white bounding box image and saves to ../img/
void DrawRectangle(Box box) {
    // utilize bitmap_image.hpp and width/length provided from lab (640, 480)
    bitmap_image image(640, 480);
    image.set_all_channels(0, 0, 0);
    image_drawer drawer(image);
    drawer.pen_width(1);
    drawer.pen_color(255, 255, 255);
    // print coordinates for bounding box
    // cout << "(" << box.vec_1.x << "," << box.vec_1.y << "),(" << box.vec_2.x << ", " << box.vec_2.y << ")" << endl;
    for (int x = box.vec_1.x; x < box.vec_2.x; x++) {
        for (int y = box.vec_1.y; y < box.vec_2.y; y++) {
            drawer.plot_pixel(x,y);
        }
    }
    image.save_image("../img/rectangle_new.bmp");
}

// Creates the triangle within the bounding box using barycentric coordinates
void DrawWhiteTriangle(Vector2* Vertices, Box bounding_box) {
    bitmap_image image(640, 480);
    image.set_all_channels(0, 0, 0);
    image_drawer drawer(image);
    drawer.pen_width(1);

    // Draw triangle using barycentric coordinates
    Line ab(Vertices[0],Vertices[1]);
    Line ac(Vertices[0],Vertices[2]);
    drawer.pen_color(255, 255, 255);
    for (int x = bounding_box.vec_1.x; x < bounding_box.vec_2.x; x++) {
        for (int y = bounding_box.vec_1.y; y < bounding_box.vec_2.y; y++) {
            Vector3 coord = BaryCoord(ab, ac, Vertices[1], Vertices[2], Vector2(x,y));
            if (isBarycentric(coord)){
                drawer.plot_pixel(x,y);
            }
        }
    }
    image.save_image("../img/white_triangle_new.bmp");
}

// Colors the triangle by using the barycentric coordinates to determine the color at the given point
void DrawColorTriangle(Vertex* Vertices, Box bounding_box) {

    bitmap_image image(640, 480);
    image.set_all_channels(0, 0, 0);
    image_drawer drawer(image);
    drawer.pen_width(1);

    Line ab(GetPosition(Vertices[0]),GetPosition(Vertices[1]));
    Line ac(GetPosition(Vertices[0]),GetPosition(Vertices[2]));

    for (int x = bounding_box.vec_1.x; x < bounding_box.vec_2.x; x++) {
        for (int y = bounding_box.vec_1.y; y < bounding_box.vec_2.y; y++) {
            Vector3 vertex = BaryCoord(ab, ac, GetPosition(Vertices[1]), GetPosition(Vertices[2]), Vector2(x,y));
            if (isBarycentric(vertex)){
                Vector3 color = GetColorBarycentric(Vertices, vertex);
                drawer.pen_color(foi(color.x),foi(color.y),foi(color.z));
                drawer.plot_pixel(x,y);
            }
        }
    }
    // output colored triangle to file in bitmap format
    image.save_image("../img/color_triangle_new.bmp");
}

int main(int argc, char** argv) {

    Vertex vertices[3];
    Vector2 position[3];

    // Get user input
    cout << "Enter 3 Vertices (enter a point as x y r g b) " << endl;
    for (int i = 0; i < 3; i++) {
        string x, y, r, g, b;
        cin >> x >> y >> r >> g >> b;
        // read in as string and convert to float
        position[i] = Vector2(stof(x),stof(y));
        vertices[i] = Vertex(stof(x),stof(y),stof(r),stof(g),stof(b));
    }

    // print out user input
    /*
    for (int i = 0; i < 3; i++) {
        cout << vertices[i] << endl;
    }
     */
    Box bounding_box(position);
    DrawRectangle(bounding_box);
    DrawWhiteTriangle(position, bounding_box);
    DrawColorTriangle(vertices, bounding_box);

    return 0;
}
