//
//  Mesh.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Mesh.h"

Mesh::Mesh(vector<Intersectable<Renderable> *> *tris, material mater, mat4 trans)
{
   tree = new Octree(tris);
   transform = trans;
   inverseTranspose = inverse(transpose(trans));
   mat = mater;
   
   triangle = NULL;
}

intersect_info<Renderable> Mesh::Intersect(ray cast)
{
   ray transformed = cast.transform_ray(inverseTranspose);
   
   intersect_info<Renderable> info = tree->Intersect(transformed);
   triangle = info.object;
   
   return info;
}

