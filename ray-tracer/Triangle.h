//
//  Triangle.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Triangle__
#define __ray_tracer__Triangle__

#include <iostream>

#include "Intersectable.h"
#include "Renderable.h"

class Triangle : public Intersectable<Renderable>, public Renderable
{
public:
   Triangle(vec3 c1, vec3 c2, vec3 c3, material mater);
   Triangle(vec3 c1, vec3 c2, vec3 c3,
            vec3 n1, vec3 n2, vec3 n3, material mater);
   
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual bounding_box Bounds() { return bounds; };
   
   virtual inline vec3 Normal(vec3 contact);
   virtual color Shade(vec3 contact, vec3 cam, const AbstractLight *light);
   virtual inline material Material() { return mat; };
   
private:
   vec3 p1, p2, p3;
   vec3 n1, n2, n3;
   
   material mat;
   bounding_box bounds;
};

#endif /* defined(__ray_tracer__Triangle__) */
