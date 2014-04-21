//
//  MeshTriangle.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 4/15/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

/*#include "MeshTriangle.h"

MeshTriangle::MeshTriangle(vector<float> pBuffer, vector<float> nBuffer,
                           int pointOne, int pointTwo, int pointThree, material mater)
{
   vec3 min, max;
   
   pointBuffer = pBuffer;
   normalBuffer = nBuffer;
   
   p1 = pointOne;
   p2 = pointTwo;
   p3 = pointThree;
   
   if (pointBuffer.at(p1*3) < pointBuffer.at(p2*3) && pointBuffer.at(p1*3) < pointBuffer.at(p3*3))
   {
      min.x = pointBuffer.at(p1*3);
      
      if (pointBuffer.at(p2*3) < pointBuffer.at(p3*3))
      {
         max.x = pointBuffer.at(p3*3);
      }
      else
      {
         max.x = pointBuffer.at(p2*3);
      }
   }
   else if (pointBuffer.at(p2*3) < pointBuffer.at(p1*3) && pointBuffer.at(p2*3) < pointBuffer.at(p3*3))
   {
      min.x = pointBuffer.at(p2*3);
      
      if (pointBuffer.at(p1*3) < pointBuffer.at(p3*3))
      {
         max.x = pointBuffer.at(p3*3);
      }
      else
      {
         max.x = pointBuffer.at(p1*3);
      }
   }
   else
   {
      min.x = pointBuffer.at(p3*3);
      
      if (pointBuffer.at(p1*3) < pointBuffer.at(p2*3))
      {
         max.x = pointBuffer.at(p2*3);
      }
      else
      {
         max.x = pointBuffer.at(p1*3);
      }
   }
   
   if (pointBuffer.at(p1*3+1) < pointBuffer.at(p2*3+1) && pointBuffer.at(p1*3+1) < pointBuffer.at(p3*3+1))
   {
      min.x = pointBuffer.at(p1*3+1);
      
      if (pointBuffer.at(p2*3+1) < pointBuffer.at(p3*3+1))
      {
         max.x = pointBuffer.at(p3*3+1);
      }
      else
      {
         max.x = pointBuffer.at(p2*3+1);
      }
   }
   else if (pointBuffer.at(p2*3+1) < pointBuffer.at(p1*3+1) && pointBuffer.at(p2*3+1) < pointBuffer.at(p3*3+1))
   {
      min.x = pointBuffer.at(p2*3+1);
      
      if (pointBuffer.at(p1*3+1) < pointBuffer.at(p3*3+1))
      {
         max.x = pointBuffer.at(p3*3+1);
      }
      else
      {
         max.x = pointBuffer.at(p1*3+1);
      }
   }
   else
   {
      min.x = pointBuffer.at(p3*3+1);
      
      if (pointBuffer.at(p1*3+1) < pointBuffer.at(p2*3+1))
      {
         max.x = pointBuffer.at(p2*3+1);
      }
      else
      {
         max.x = pointBuffer.at(p1*3+1);
      }
   }
   
   if (pointBuffer.at(p1*3+2) < pointBuffer.at(p2*3+2) && pointBuffer.at(p1*3+2) < pointBuffer.at(p3*3+2))
   {
      min.x = pointBuffer.at(p1*3+2);
      
      if (pointBuffer.at(p2*3+2) < pointBuffer.at(p3*3+2))
      {
         max.x = pointBuffer.at(p3*3+2);
      }
      else
      {
         max.x = pointBuffer.at(p2*3+2);
      }
   }
   else if (pointBuffer.at(p2*3+2) < pointBuffer.at(p1*3+2) && pointBuffer.at(p2*3+2) < pointBuffer.at(p3*3+2))
   {
      min.x = pointBuffer.at(p2*3+2);
      
      if (pointBuffer.at(p1*3+2) < pointBuffer.at(p3*3+2))
      {
         max.x = pointBuffer.at(p3*3+2);
      }
      else
      {
         max.x = pointBuffer.at(p1*3+2);
      }
   }
   else
   {
      min.x = pointBuffer.at(p3*3+2);
      
      if (pointBuffer.at(p1*3+2) < pointBuffer.at(p2*3+2))
      {
         max.x = pointBuffer.at(p2*3+2);
      }
      else
      {
         max.x = pointBuffer.at(p1*3+2);
      }
   }
   
   bounds = bounding_box(min, max);
   
   mat = mater;
}

intersect_info<Renderable> Triangle::Intersect(ray cast)
{
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, MISS);
   
   vec3 t1 = vec3(pointBuffer.at(p1*3), pointBuffer.at(p1*3+1), pointBuffer.at(p1*3+2));
   vec3 t2 = vec3(pointBuffer.at(p2*3), pointBuffer.at(p2*3+1), pointBuffer.at(p2*3+2));
   vec3 t3 = vec3(pointBuffer.at(p3*3), pointBuffer.at(p3*3+1), pointBuffer.at(p3*3+2));
   
   vec3 edge1 = t2 - t1;
   vec3 edge2 = t3 - t1;
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

intersect_info<Renderable> Triangle::SafeIntersect(ray cast)
{
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, MISS);
   
   vec3 t1 = vec3(pointBuffer.at(p1*3), pointBuffer.at(p1*3+1), pointBuffer.at(p1*3+2));
   vec3 t2 = vec3(pointBuffer.at(p2*3), pointBuffer.at(p2*3+1), pointBuffer.at(p2*3+2));
   vec3 t3 = vec3(pointBuffer.at(p3*3), pointBuffer.at(p3*3+1), pointBuffer.at(p3*3+2));
   
   vec3 edge1 = t2 - t1;
   vec3 edge2 = t3 - t1;
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
   vec3 t1 = vec3(pointBuffer.at(p1*3), pointBuffer.at(p1*3+1), pointBuffer.at(p1*3+2));
   vec3 t2 = vec3(pointBuffer.at(p2*3), pointBuffer.at(p2*3+1), pointBuffer.at(p2*3+2));
   vec3 t3 = vec3(pointBuffer.at(p3*3), pointBuffer.at(p3*3+1), pointBuffer.at(p3*3+2));
   vec3 n1 = vec3(normalBuffer.at(p1*3), normalBuffer.at(p1*3+1), normalBuffer.at(p1*3+2));
   vec3 n2 = vec3(normalBuffer.at(p2*3), normalBuffer.at(p2*3+1), normalBuffer.at(p2*3+2));
   vec3 n3 = vec3(normalBuffer.at(p3*3), normalBuffer.at(p3*3+1), normalBuffer.at(p3*3+2));
   
   vec3 p1p2, p1p3, p2p3, p1x, p2x;
   vec4 nor1, nor2, nor3, normal;
   
   nor1 = vec4(n1, 0.0f);
   nor2 = vec4(n2, 0.0f);
   nor3 = vec4(n3, 0.0f);
   
   p1p2 = t2 - t1;
   p1p3 = t3 - t1;
   p2p3 = t3 - t2;
   p1x = contact - t1;
   p2x = contact - t2;
   
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
}*/