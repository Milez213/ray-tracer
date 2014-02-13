//
//  Triangle.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Triangle.h"

Triangle::Triangle(vec3 c1, vec3 c2, vec3 c3, material mater)
{
   vec3 min, max;
   
   p1 = c1;
   p2 = c2;
   p3 = c3;
   
   if (p1.x < p2.x && p1.x < p3.x)
   {
      min.x = p1.x;
      
      if (p2.x < p3.x)
      {
         max.x = p3.x;
      }
      else
      {
         max.x = p2.x;
      }
   }
   else if (p2.x < p1.x && p2.x < p3.x)
   {
      min.x = p2.x;
      
      if (p1.x < p3.x)
      {
         max.x = p3.x;
      }
      else
      {
         max.x = p1.x;
      }
   }
   else
   {
      min.x = p3.x;
      
      if (p1.x < p2.x)
      {
         max.x = p2.x;
      }
      else
      {
         max.x = p1.x;
      }
   }
   
   if (p1.y < p2.y && p1.y < p3.y)
   {
      min.y = p1.y;
      
      if (p2.y < p3.y)
      {
         max.y = p3.y;
      }
      else
      {
         max.y = p2.y;
      }
   }
   else if (p2.y < p1.y && p2.y < p3.y)
   {
      min.y = p2.y;
      
      if (p1.y < p3.y)
      {
         max.y = p3.y;
      }
      else
      {
         max.y = p1.y;
      }
   }
   else
   {
      min.y = p3.y;
      
      if (p1.y < p2.y)
      {
         max.y = p2.y;
      }
      else
      {
         max.y = p1.y;
      }
   }
   
   if (p1.z < p2.z && p1.z < p3.z)
   {
      min.z = p1.z;
      
      if (p2.z < p3.z)
      {
         max.z = p3.z;
      }
      else
      {
         max.z = p2.z;
      }
   }
   else if (p2.z < p1.z && p2.z < p3.z)
   {
      min.z = p2.z;
      
      if (p1.z < p3.z)
      {
         max.z = p3.z;
      }
      else
      {
         max.z = p1.z;
      }
   }
   else
   {
      min.z = p3.z;
      
      if (p1.z < p2.z)
      {
         max.z = p2.z;
      }
      else
      {
         max.z = p1.z;
      }
   }
   
   bounds = bounding_box(min, max);
   
   vec3 r1 = normalize(p2 - p1);
   vec3 r2 = normalize(p3 - p1);
   
   vec3 norm = -cross(r2, r1);
   
   n1 = n2 = n3 = normalize(vec3(norm.x, norm.y, norm.z));
   
   mat = mater;
}

Triangle::Triangle(vec3 c1, vec3 c2, vec3 c3,
                   vec3 m1, vec3 m2, vec3 m3, material mater)
{
   vec3 min, max;
   
   p1 = c1;
   p2 = c2;
   p3 = c3;
   
   if (p1.x < p2.x && p1.x < p3.x)
   {
      min.x = p1.x;
      
      if (p2.x < p3.x)
      {
         max.x = p3.x;
      }
      else
      {
         max.x = p2.x;
      }
   }
   else if (p2.x < p1.x && p2.x < p3.x)
   {
      min.x = p2.x;
      
      if (p1.x < p3.x)
      {
         max.x = p3.x;
      }
      else
      {
         max.x = p1.x;
      }
   }
   else
   {
      min.x = p3.x;
      
      if (p1.x < p2.x)
      {
         max.x = p2.x;
      }
      else
      {
         max.x = p1.x;
      }
   }
   
   if (p1.y < p2.y && p1.y < p3.y)
   {
      min.y = p1.y;
      
      if (p2.y < p3.y)
      {
         max.y = p3.y;
      }
      else
      {
         max.y = p2.y;
      }
   }
   else if (p2.y < p1.y && p2.y < p3.y)
   {
      min.y = p2.y;
      
      if (p1.y < p3.y)
      {
         max.y = p3.y;
      }
      else
      {
         max.y = p1.y;
      }
   }
   else
   {
      min.y = p3.y;
      
      if (p1.y < p2.y)
      {
         max.y = p2.y;
      }
      else
      {
         max.y = p1.y;
      }
   }
   
   if (p1.z < p2.z && p1.z < p3.z)
   {
      min.z = p1.z;
      
      if (p2.z < p3.z)
      {
         max.z = p3.z;
      }
      else
      {
         max.z = p2.z;
      }
   }
   else if (p2.z < p1.z && p2.z < p3.z)
   {
      min.z = p2.z;
      
      if (p1.z < p3.z)
      {
         max.z = p3.z;
      }
      else
      {
         max.z = p1.z;
      }
   }
   else
   {
      min.z = p3.z;
      
      if (p1.z < p2.z)
      {
         max.z = p2.z;
      }
      else
      {
         max.z = p1.z;
      }
   }
   
   bounds = bounding_box(min, max);
   
   n1 = m1;
   n2 = m2;
   n3 = m3;
   
   mat = mater;
}

intersect_info<Renderable> Triangle::Intersect(ray cast)
{
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, MISS);
   
   vec3 edge1 = p2 - p1;
   vec3 edge2 = p3 - p1;
   vec3 origin = cast.origin - p1;
   
   //Commonly used cross products are only calculated once (substitutions)
   vec3 sub1 = cross(cast.direction, edge2);
   vec3 sub2 = cross(origin, edge1);
   
   vec3 final = (1 / dot(sub1, edge1)) * vec3(dot(sub2, edge2),
                                              dot(sub1, origin),
                                              dot(sub2, cast.direction));
   
   if (final.y >= 0.0f && final.z >= 0.0f && final.y + final.z <= 1.0f && final.x > 0.0f)
   {
      info.object = this;
      info.time = final.x;
   }
   
   return info;
}

vec3 Triangle::Normal(vec3 contact)
{
   vec3 p1p2, p1p3, p2p3, p1x, p2x;
   vec4 nor1, nor2, nor3, normal;
   
   nor1 = vec4(n1, 0.0f);
   nor2 = vec4(n2, 0.0f);
   nor3 = vec4(n3, 0.0f);
   
   p1p2 = p2 - p1;
   p1p3 = p3 - p1;
   p2p3 = p3 - p2;
   p1x = contact - p1;
   p2x = contact - p2;
   
   float a12, a13, a23, total;
   
   a12 = length(cross(p1p2, p1x)) / 2;
   a13 = length(cross(p1p3, p1x)) / 2;
   a23 = length(cross(p2p3, p2x)) / 2;
   
   total = a12 + a13 + a23;
   
   normal = nor1 * (a23 / total) + nor2 * (a13 / total) + nor3 * (a12 / total);
   
   return normalize(vec3(normal.x, normal.y, normal.z));
}

color Triangle::Shade(vec3 contact, vec3 cam, const AbstractLight *light)
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