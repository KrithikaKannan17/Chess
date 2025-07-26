#include "window.h"
#include <iostream>
#include <vector>

const int BORDER_SIZE = 2;
const int COORD_TEXT_SZ = 12;
const int PIXEL_SZ = 256;

using namespace std;

Xwindow::Xwindow(int width, int height) : width(width), height(height) { // constructor

    d = XOpenDisplay(NULL);
    if (d == NULL) {
        cerr << "Cannot open display" << endl;
        return;
    }
    
    s = DefaultScreen(d); // get default screen
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, BORDER_SIZE, // create simple window
                           BlackPixel(d, s), WhitePixel(d, s)); // set black and white pixels
    
    XSelectInput(d, w, ExposureMask | KeyPressMask); // select input    
    XMapWindow(d, w); // map window
    
    gc = XCreateGC(d, w, 0, NULL); // create graphics context
    colours[White] = WhitePixel(d, s); // set white pixel
    colours[Black] = BlackPixel(d, s); // set black pixel
    Colormap colormap = DefaultColormap(d, s); // get default colormap
    XColor color; // create color
    

    color.red = 0xF0 * PIXEL_SZ; // set red
    color.green = 0xD9 * PIXEL_SZ; // set green
    color.blue = 0xB5 * PIXEL_SZ; // set blue
    if (XAllocColor(d, colormap, &color)) { // allocate color
        colours[LightSquare] = color.pixel; // set light square
    } else {
        colours[LightSquare] = colours[White];
    }
    

    color.red = 0xB5 * PIXEL_SZ;
    color.green = 0x88 * PIXEL_SZ;
    color.blue = 0x63 * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) { // allocate color
        colours[DarkSquare] = color.pixel;
    } else {
        colours[DarkSquare] = colours[Black];
    }
    
    // board border
    color.red = 0x8B * PIXEL_SZ;
    color.green = 0x45 * PIXEL_SZ;
    color.blue = 0x13 * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[BoardBorder] = color.pixel;
    } else {
        colours[BoardBorder] = colours[Black];
    }

    color.red = 0x6F * PIXEL_SZ;
    color.green = 0x4E * PIXEL_SZ;
    color.blue = 0x37 * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[CoordText] = color.pixel;
    } else {
        colours[CoordText] = colours[Black];
    }
    
    color.red = 0xFF * PIXEL_SZ;
    color.green = 0xFF * PIXEL_SZ;
    color.blue = 0x99 * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[HighlightYellow] = color.pixel;
    } else {
        colours[HighlightYellow] = colours[White];
    }
    
    color.red = 0x90 * PIXEL_SZ;
    color.green = 0xEE * PIXEL_SZ;
    color.blue = 0x90 * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[HighlightGreen] = color.pixel;
    } else {
        colours[HighlightGreen] = colours[White];
    }
    
    color.red = 0xFF * PIXEL_SZ;
    color.green = 0x6B * PIXEL_SZ;
    color.blue = 0x6B * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[CheckRed] = color.pixel;
    } else {
        colours[CheckRed] = colours[Black];
    }
    
    color.red = 0xF0 * PIXEL_SZ;
    color.green = 0xF8 * PIXEL_SZ;
    color.blue = 0xD0 * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[LightSquareGreen] = color.pixel;
    } else {
        colours[LightSquareGreen] = colours[White];
    }
    
    color.red = 0x7A * PIXEL_SZ;
    color.green = 0x9A * PIXEL_SZ;
    color.blue = 0x5A * PIXEL_SZ;
    if (XAllocColor(d, colormap, &color)) {
        colours[DarkSquareGreen] = color.pixel;
    } else {
        colours[DarkSquareGreen] = colours[Green];
    }
    
    colours[PieceWhite] = colours[White]; // set piece white
    colours[PieceBlack] = colours[Black]; // set piece black
    
    colours[Red] = colours[CheckRed]; // set red
    colours[Green] = colours[HighlightGreen]; // set green
    colours[Blue] = colours[Black]; // set blue
    colours[Cyan] = colours[Black]; // set cyan
    colours[Yellow] = colours[HighlightYellow]; // set yellow
    colours[Magenta] = colours[Black]; // set magenta
    colours[Orange] = colours[Black]; // set orange
    colours[Brown] = colours[DarkSquare]; // set brown
    
    XFlush(d);
}

Xwindow::~Xwindow() {
    if (d) {
        XFreeGC(d, gc);
        XCloseDisplay(d);
    }
}

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
    XSetForeground(d, gc, colours[colour]); // set foreground
    XFillRectangle(d, w, gc, x, y, width, height); // fill rectangle
}

void Xwindow::drawString(int x, int y, const string& msg, int colour) {
    XSetForeground(d, gc, colours[colour]); // set foreground
    XDrawString(d, w, gc, x, y, msg.c_str(), msg.length()); // draw string
}

void Xwindow::drawStringCentered(int x, int y, int width, int height, const string& msg, int colour) {
    XSetForeground(d, gc, colours[colour]); // set foreground
    
    int centerX = x + width / 2 - (msg.length() * COORD_TEXT_SZ) / 2;  // Assuming 6px per character
    int centerY = y + height / 2 + COORD_TEXT_SZ;  // Assuming 12px font height
    
    XDrawString(d, w, gc, centerX, centerY, msg.c_str(), msg.length()); // draw string
}

void Xwindow::setFont(const string& fontName) { // set font
    XFontStruct* font = XLoadQueryFont(d, fontName.c_str());
    if (font) {
        XSetFont(d, gc, font->fid); // set font
        XFreeFont(d, font); // free font
    }
}

void Xwindow::drawLine(int x1, int y1, int x2, int y2, int colour) {
    XSetForeground(d, gc, colours[colour]); // set foreground
    XDrawLine(d, w, gc, x1, y1, x2, y2); // draw line
}

void Xwindow::drawRectangle(int x, int y, int width, int height, int colour) {
    XSetForeground(d, gc, colours[colour]); // set foreground
    XDrawRectangle(d, w, gc, x, y, width, height); // draw rectangle
}

void Xwindow::fillPolygon(const int* points, int npoints, int colour) { // lowkey never used
    XSetForeground(d, gc, colours[colour]); // set foreground
    
    std::vector<XPoint> xpoints(npoints);
    for (int i = 0; i < npoints; i++) {
        xpoints[i].x = points[i * 2];     // x coordinate
        xpoints[i].y = points[i * 2 + 1]; // y coordinate
    }
    
    XFillPolygon(d, w, gc, xpoints.data(), npoints, Convex, CoordModeOrigin); // fill polygon
    
}

void Xwindow::clear() { // clear window
    XClearWindow(d, w);
}

void Xwindow::flush() { // flush
    XFlush(d);
} 
