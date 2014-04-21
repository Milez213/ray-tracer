//
//  MeshTriangle.h
//  ray-tracer
//
//  Created by Bryan Bell on 4/15/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__MeshTriangle__
#define __ray_tracer__MeshTriangle__

#include <iostream>

/*#include "Intersectable.h"
#include "Renderable.h"

class MeshTriangle : public Intersectable<Renderable>, public Renderable
{
public:
   MeshTriangle(vector<float> pointBuffer, vector<float> normalBuffer,
                int pointOne, int pointTwo, int pointThree, material mater);
   
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual intersect_info<Renderable> SafeIntersect(ray cast);
   virtual bounding_box Bounds() { return bounds; };
   
   virtual inline vec3 Normal(vec3 contact);
   virtual color Shade(vec3 contact, vec3 cam, const AbstractLight *light);
   virtual inline material Material() { return mat; };
   
private:
   vector<float> pointBuffer;
   vector<float> normalBuffer;
   int p1, p2, p3;
   
   material mat;
   bounding_box bounds;
};*/

#endif /* defined(__ray_tracer__MeshTriangle__) */
