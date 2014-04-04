//
//  Sphere.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Sphere__
#define __ray_tracer__Sphere__

#include <iostream>

#include "Intersectable.h"
#include "Renderable.h"

#include "AbstractShader.h"

class Sphere : public Intersectable<Renderable>, public Renderable
{
public:
   Sphere(vec3 center, float radius, material mat);
   Sphere(vec3 center, float rad, material mtr, mat4 trans);
   
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual intersect_info<Renderable> SafeIntersect(ray cast);
   virtual inline bounding_box Bounds() { return bounds; };
   
   virtual inline vec3 Normal(vec3 contact);
   virtual color Shade(vec3 contact, vec3 cam, const AbstractLight *light);
   virtual inline material Material() { return mat; };
   
private:
   vec3 position;
   float radius;
   
   mat4 transform;
   mat4 inverseTranspose;
   
   material mat;
   bounding_box bounds;
};

#endif /* defined(__ray_tracer__Sphere__) */
