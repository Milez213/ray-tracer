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
   
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual bounding_box Bounds() { return tree->Bounds(); };
   
   virtual inline vec3 Normal(vec3 contact) { return triangle->Normal(contact); };
   virtual color Shade(vec3 contact, vec3 cam, const AbstractLight *light) { return triangle->Shade(contact, cam, light); };
   virtual inline material Material() { return mat; };
   
private:
   Octree *tree;

   mat4 transform;
   mat4 inverseTranspose;
   
   material mat;
   Renderable *triangle;
};

#endif /* defined(__ray_tracer__Mesh__) */
