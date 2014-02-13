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
}

ray ***Camera::CastRays()
{
   ray ***cast = (ray ***)malloc(sizeof(ray **) * width);
   
   int raysPerPixel = powf(2, antialiasing);
   
   for (int i = 0; i < width; i++)
   {
      cast[i] = (ray **)malloc(sizeof(ray *) * height);
      for (int j = 0; j < height; j++)
      {
         cast[i][j] = (ray *)malloc(sizeof(ray) * raysPerPixel);
         for (int k = 0; k < raysPerPixel; k++)
         {
            
         }
      }
   }
   
   return cast;
}

vec3 Camera::Position()
{
   return vec3(0.0f);
}