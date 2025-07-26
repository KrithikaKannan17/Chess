#ifndef WINDOW_H
#define WINDOW_H

#include <X11/Xlib.h>
#include <string>

using namespace std;

class Xwindow { // window class
    Display *d; // display
    Window w; // window
    int s; // screen
    GC gc; // graphics context
    unsigned long colours[22]; // colours
    int width, height; // width and height

public:
    Xwindow(int width = 500, int height = 500); // constructor
    ~Xwindow(); // destructor

    enum Color { // color enum
        White = 0,
        Black,
        Red,
        Green,
        Blue,
        Cyan,
        Yellow,
        Magenta,
        Orange,
        Brown,
        LightSquare,
        DarkSquare,
        LightSquareGreen,
        DarkSquareGreen,
        BoardBorder,
        CoordText,
        HighlightYellow,
        HighlightGreen,
        CheckRed,
        PieceWhite,
        PieceBlack
    };

    void fillRectangle(int x, int y, int width, int height, int colour = Black); // fill rectangle
    void drawString(int x, int y, const string& msg, int colour = Black); // draw string
    void drawLine(int x1, int y1, int x2, int y2, int colour = Black); // draw line
    void drawRectangle(int x, int y, int width, int height, int colour = Black); // draw rectangle
    void fillPolygon(const int* points, int npoints, int colour = Black); // fill polygon
    
    void drawStringCentered(int x, int y, int width, int height, const string& msg, int colour = Black); // draw string centered
    void setFont(const string& fontName); // set font
    
    void clear(); // clear
    void flush(); // flush
    
    int getWidth() const { return width; } // get width
    int getHeight() const { return height; } // get height
};

#endif 
// followed tutorial and wiki to create window
