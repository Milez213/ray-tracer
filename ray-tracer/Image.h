/**
 * Bob Somers
 * rsomers@calpoly.edu
 * CPE 473, Winter 2010
 * Cal Poly, San Luis Obispo
 */

/* Will Starr, Bryan Bell, Alexa Francis
 * Project 2 - Part B
 * Winter 2013
 * Image.h
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct color_t {
   float r;
   float g;
   float b;
   float a;
   
   color_t(float red, float green, float blue, float alpha)
   {
      r = red;
      g = green;
      b = blue;
      a = alpha;
   }
};

class Image {
public:
    Image(int width, int height);
    ~Image();

    /* if scale_color is true, the output targa will have its color space scaled
     * to the global max, otherwise it will be clamped at 1.0 */
    void WriteTga(char *outfile, bool scale_color = true);

    void GenTestPattern();

    /* property accessors */
    color_t pixel(int x, int y);
    void pixel(int x, int y, color_t pxl);
    int width() const { return _width; }
    int height() const { return _height; }
    double max() const { return _max; }

private:
    int _width;
    int _height;
    color_t **_pixmap;
    double _max;
};

#endif

