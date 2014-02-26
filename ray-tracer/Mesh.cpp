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

vec3 Mesh::Normal(vec3 contact)
{
   vec3 transformedContact = transform_point(contact, inverse(transform));
   vec3 transformedNormal = transform_normal(triangle->Normal(transformedContact), inverse(transform));
   
   return transformedNormal;
}

intersect_info<Renderable> Mesh::Intersect(ray cast)
{
   ray transformed = cast.transform_ray(inverse(transform));
   
   intersect_info<Renderable> info = tree->Intersect(transformed);
   if (info.object != NULL)
   {
      triangle = info.object;
   }
   
   return intersect_info<Renderable>(this, info.time);
}

color Mesh::Shade(vec3 contact, vec3 cam, const AbstractLight *light)
{
   color shade;
	
	vec3 normal = Normal(contact);
	normal = normalize(normal);
   
	vec3 lightDirection = light->GetDirection(contact);
   
	float dirNor = dot(lightDirection, normal);
   if (dirNor < 0.0f)
   {
      dirNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * mat.diffuse * dirNor;
	
	vec3 cameraDirection = normalize(cam - contact);
	vec3 difference = normalize(lightDirection + cameraDirection);
	
	float difNor = dot(difference, normal);
   if (difNor < 0.0f)
   {
      difNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * pow(difNor, 1.0f/mat.roughness) * mat.specular;
   
	return shade;
}

