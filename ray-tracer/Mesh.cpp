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
   lastHit = NULL;
}

vec3 Mesh::Normal(vec3 contact)
{
   vec3 transformedContact;
   vec3 transformedNormal;
   
   transformedContact = transform_point(contact, inverse(transform));
   transformedNormal = transform_normal(triangle->Normal(transformedContact), inverse(transform));
   
   return transformedNormal;
}

intersect_info<Renderable> Mesh::Intersect(ray cast)
{
   ray transformed = cast.transform_ray(inverse(transform));
   float time = INF;
   
   intersect_info<Renderable> info;
   
   if (tree)
   {
      info = tree->Intersect(transformed);
      if (info.object != NULL)
      {
         triangle = info.object;
         time = info.time;
         lastHit = NULL;
      }
   }
   
   return intersect_info<Renderable>(this, time);
}

intersect_info<Renderable> Mesh::SafeIntersect(ray cast)
{
   ray transformed = cast.transform_ray(inverse(transform));
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

color Mesh::Shade(vec3 contact, vec3 cam, const AbstractLight *light)
{
   color shade;
   material triMat = triangle->Material();
	
	vec3 normal = Normal(contact);
	normal = normalize(normal);
   
	vec3 lightDirection = light->GetDirection(contact);
   
	float dirNor = dot(lightDirection, normal);
   if (dirNor < 0.0f)
   {
      dirNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * triMat.diffuse * dirNor;
	
	vec3 cameraDirection = normalize(cam - contact);
	vec3 difference = normalize(lightDirection + cameraDirection);
	
	float difNor = dot(difference, normal);
   if (difNor < 0.0f)
   {
      difNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * pow(difNor, 1.0f/triMat.roughness) * triMat.specular;
   
	return shade;

}

