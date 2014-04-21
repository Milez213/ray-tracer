//
//  Mesh.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Mesh__
#define __ray_tracer__Mesh__

#include <iostream>
#include <vector>

#include "material.h"
#include "AbstractShader.h"

#include "Renderable.h"
#include "Intersectable.h"

#include "Octree.h"

#include "Triangle.h"

using namespace glm;
using namespace std;

class Mesh : public Intersectable<Renderable>, public Renderable
{
public:
   Mesh(vector<Intersectable<Renderable> *> *triangles, material mat, mat4 transform);
   
#pragma mark Intersectable Overrides
   
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual intersect_info<Renderable> SafeIntersect(ray cast);
   virtual bounding_box Bounds() { return tree->Bounds(); };
   
#pragma mark Renderable Overrides

   virtual inline vec3 Normal(vec3 contact);
   virtual inline material Material() { return triangle->Material(); };
   
private:
   inline mat4 Transform() { return transform; };
   
   Octree *tree;

   mat4 transform;
   mat4 invers;
   
   material mat;
   Renderable *triangle;
};

#endif /* defined(__ray_tracer__Mesh__) */
