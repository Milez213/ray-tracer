//
//  Sphere.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Sphere.h"

Sphere::Sphere(vec3 center, float rad, material mtr)
{
   position = center;
   radius = rad;
   mat = mtr;
   
   bounds = bounding_box(position - radius, position + radius);
}

intersect_info<Renderable> Sphere::Intersect(ray cast)
{
   intersect_info<Renderable> hit;
   
	float a = dot(cast.direction, cast.direction);
	float b = 2.0f * dot(cast.origin - position, cast.direction);
	float c = dot(cast.origin - position, cast.origin - position) - powf(radius, 2);
	
	float discriminant = powf(b, 2) - 4 * a * c;
	
	if (discriminant >= 0.0f) {
		discriminant = sqrt(discriminant);
		float q;
		
		if (b < 0)
        	q = -(b - discriminant)/2.0;
    	else
        	q = -(b + discriminant)/2.0;
      
      float t0 = q / a;
    	float t1 = c / q;
    	
    	if (t0 > t1)
      {
	      float temp = t0;
	      t0 = t1;
	      t1 = temp;
	   }
      
	   if (t0 < 0)
      {
	      hit.object = this;
         hit.time = t1;
	   }
      else
      {
	      hit.object = this;
         hit.time = t0;
	   }
	}
	
	return hit;
}

vec3 Sphere::Normal(vec3 contact)
{
   return normalize(contact - position);
}

color Sphere::Shade(vec3 contact, vec3 cam, const AbstractLight *light)
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