//
//  Camera.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Camera__
#define __ray_tracer__Camera__

#include <iostream>

#include "glm/glm.hpp"

#include "ray.h"

using namespace glm;

class Camera
{
public:
   Camera(vec3 eye, vec3 lookAt, vec3 up);
   ray ***CastRays() const;
   void CleanUpRays(ray ***cast) const;
   
   inline vec3 Position() const { return eye; };
   
   inline int Width() const { return width; };
   inline void SetWidth(int w) { width = w; };
   inline int Height() const { return height; };
   inline void SetHeight(int h) { height = h; };
   inline int AntiAliasing() const { return antialiasing; };
   inline void SetAntiAliasing(int level) { antialiasing = level; };
   
private:
   vec3 eye;
   vec3 lookAt;
   vec3 up;
   
   int width;
   int height;
   int antialiasing;
};

#endif /* defined(__ray_tracer__Camera__) */
