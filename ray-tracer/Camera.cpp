//
//  Camera.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Camera.h"

Camera::Camera(vec3 e, vec3 l, vec3 u)
{
   eye = e;
   lookAt = l;
   up = u;
   
   width = 640;
   height = 480;
   antialiasing = 1;
}

ray ***Camera::CastRays() const
{
   ray ***cast = (ray ***)malloc(sizeof(ray **) * width);
   
   int raysPerPixel = powf(antialiasing, 2);
   
   float xPixelCenter;
   float yPixelCenter;
   float xRayPosition;
   float yRayPosition;
   float sectionSize = 1.0f / (float)antialiasing;
   
   vec3 w;
   vec3 u;
   vec3 v;
   vec3 direction;
   
   for (int i = 0; i < width; i++)
   {
      cast[i] = (ray **)malloc(sizeof(ray *) * height);
      for (int j = 0; j < height; j++)
      {
         cast[i][j] = (ray *)malloc(sizeof(ray) * raysPerPixel);
         
         xPixelCenter = (float)i - 0.5f;
         yPixelCenter = (float)j - 0.5f;
         
         for (unsigned int x = 0; x < antialiasing; x++) {
            xRayPosition = (xPixelCenter + sectionSize * x) / (float)width - 0.5f;
            for (unsigned int y = 0; y < antialiasing; y++) {
               yRayPosition = ((yPixelCenter + sectionSize * y) / (float)height) * ((float)height/(float)width) - ((float)height/(float)width) / 2;
               
               w = normalize(lookAt - eye);
               u = normalize(cross(w, up));
               v = normalize(cross(u, w));
               
               direction = normalize(w + xRayPosition * u + yRayPosition * v);
               
               cast[i][j][x * antialiasing + y] = ray(eye, direction);
            }
         }
         
      }
   }
   
   return cast;
}

void Camera::CleanUpRays(ray ***cast) const
{
   for (int i = 0; i < width; i++)
   {
      for (int j = 0; j < height; j++)
      {
         free(cast[i][j]);
      }
      free(cast[i]);
   }
   free(cast);
}