//
//  Mesh.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Mesh.h"

#include "PhongShader.h"

Mesh::Mesh(vector<Intersectable<Renderable> *> *tris, material mater, mat4 trans)
{
   tree = new Octree(tris);
   transform = trans;
   invers = inverse(trans);
   mat = mater;
   
   triangle = NULL;
   
   SetShader(PhongShader::GetInstance());
}

vec3 Mesh::Normal(vec3 contact)
{
   vec3 transformedContact;
   vec3 transformedNormal;
   
   transformedContact = transform_point(contact, invers);
   transformedNormal = transform_normal(triangle->Normal(transformedContact), transpose(transform));
   
   return transformedNormal;
}

intersect_info<Renderable> Mesh::Intersect(ray cast)
{
   ray transformed = cast.transform_ray(invers);
   float time = INF;
   
   intersect_info<Renderable> info;
   
   if (tree)
   {
      info = tree->Intersect(transformed);
      if (info.object != NULL)
      {
         triangle = info.object;
         time = info.time;
      }
   }
   
   return intersect_info<Renderable>(this, time);
}

intersect_info<Renderable> Mesh::SafeIntersect(ray cast)
{
   ray transformed = cast.transform_ray(invers);
   Renderable *object = NULL;
   float time = INF;
   
   intersect_info<Renderable> info;
   
   if (tree)
   {
      info = tree->Intersect(transformed);
      if (info.object != NULL)
      {
         object = info.object;
         time = info.time;
      }
   }
   
   return intersect_info<Renderable>(object, time);
}

