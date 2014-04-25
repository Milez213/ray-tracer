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
   
   transform = mat4(1.0f);
   inverseTranspose = inverse(transpose(transform));
   
   bounds = bounding_box(position - radius, position + radius);
}

Sphere::Sphere(vec3 center, float rad, material mtr, mat4 trans)
{
   position = center;
   radius = rad;
   mat = mtr;
   
   transform = trans;
   inverseTranspose = inverse(transpose(transform));
   
   bounds = bounding_box(position - radius, position + radius);
}

intersect_info<Renderable> Sphere::Intersect(ray cast)
{
   intersect_info<Renderable> hit;
   
   ray transformed = cast.transform_ray(inverseTranspose);
   
	float a = dot(transformed.direction, transformed.direction);
	float b = 2.0f * dot(transformed.origin - position, transformed.direction);
	float c = dot(transformed.origin - position, transformed.origin - position) - powf(radius, 2.0f);
	
	float discriminant = powf(b, 2) - 4 * a * c;
	
	if (discriminant >= 0.0f) {
		discriminant = sqrt(discriminant);
		float q;
		
		if (b < 0.0f)
        	q = -(b - discriminant)/2.0f;
    	else
        	q = -(b + discriminant)/2.0f;
      
      float t0 = q / a;
    	float t1 = c / q;
    	
    	if (t0 > t1)
      {
	      float temp = t0;
	      t0 = t1;
	      t1 = temp;
	   }
      
	   if (t0 < 0.0f)
      {
	      hit.object = this;
         hit.time = t1;
	   }
      else if (t1 > 0.0f)
      {
	      hit.object = this;
         hit.time = t0;
	   }
	}
	
	return hit;
}

intersect_info<Renderable> Sphere::SafeIntersect(ray cast)
{
   intersect_info<Renderable> hit;
   
   ray transformed = cast.transform_ray(inverseTranspose);
   
	float a = dot(transformed.direction, transformed.direction);
	float b = 2.0f * dot(transformed.origin - position, transformed.direction);
	float c = dot(transformed.origin - position, transformed.origin - position) - powf(radius, 2.0f);
	
	float discriminant = powf(b, 2) - 4 * a * c;
	
	if (discriminant >= 0.0f) {
		discriminant = sqrt(discriminant);
		float q;
		
		if (b < 0.0f)
        	q = -(b - discriminant)/2.0f;
    	else
        	q = -(b + discriminant)/2.0f;
      
      float t0 = q / a;
    	float t1 = c / q;
    	
    	if (t0 > t1)
      {
	      float temp = t0;
	      t0 = t1;
	      t1 = temp;
	   }
      
	   if (t0 < 0.0f)
      {
	      hit.object = this;
         hit.time = t1;
	   }
      else if (t1 > 0.0f)
      {
	      hit.object = this;
         hit.time = t0;
	   }
	}
	
	return hit;
}

vec3 Sphere::Normal(vec3 contact)
{
   vec3 transformedContact = transform_point(contact, inverseTranspose);
   
   return normalize(transformedContact - position);
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
	
	shade += light->Intensity(contact, normal) * pow(difNor, 1.0f/mat.shininess) * mat.specular;
   
	return shade;
}