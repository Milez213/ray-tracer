//
//  ray.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_ray_h
#define ray_tracer_ray_h

#include "glm/glm.hpp"

#define MISS -1
#define INF 10000.0f
#define STEP 0.01f

using namespace glm;

#pragma mark GLM Transform Extensions

inline vec3 transform_point(vec3 p, mat4 transform)
{
   vec4 transformed = vec4(p, 1.0f) * transform;
   
   if (transformed.w != 1.0f)
   {
      transformed = transformed / transformed.w;
   }
   
   return vec3(transformed.x, transformed.y, transformed.z);
}

inline vec3 transform_vector(vec3 v, mat4 transform)
{
   vec4 transformed = vec4(v, 0.0f) * transform;
   
   return vec3(transformed.x, transformed.y, transformed.z);
}

inline vec3 transform_normal(vec3 n, mat4 transform)
{
   vec4 transformed = vec4(n, 0.0f) * inverse(transform);
   
   return vec3(transformed.x, transformed.y, transformed.z);
}

#pragma mark Ray Struct Definition

struct ray
{
   vec3 origin;
   vec3 direction;
   
   inline ray()
   {
      origin = vec3(0.0f);
      direction = vec3(0.0f);
   }
   
   inline ray(vec3 orig, vec3 dir)
   {
      origin = orig;
      direction = dir;
   }
   
   inline ray refract_ray(vec3 contact, vec3 normal, float iorIn, float iorOut)
   {
      ray refracted = ray(contact, direction);
      
      if (dot(normal, direction) > 0.0f)
         normal = -normal;
      
      float root = 1 - pow(iorIn/iorOut, 2) *
      (1 - pow(dot(direction, normal), 2));
      
      if (root <= 0) {
         refracted.direction = vec3(0.0f);
      }
      
      refracted.direction = normalize((iorIn/iorOut) *
                                     (direction - normal *
                                     dot(direction, normal)) -
                                     normal * sqrt(root));
      refracted.origin = contact + (refracted.direction * STEP);
      
      return refracted;
   }
   
   inline ray reflect_ray(vec3 contact, vec3 normal)
   {
      ray reflected = ray(origin, direction);
      
      if (dot(normal, direction) > 0.0f)
         normal = -normal;
      
      reflected.direction = normalize(direction - 2 * dot(direction, normal) * normal);
      reflected.origin = contact + (reflected.direction * STEP);
      
      return reflected;
   }
   
   inline ray transform_ray(mat4 transform)
   {
      vec3 transformedOrigin = transform_point(origin, transform);
      vec3 transformedDirection = transform_vector(direction, transform);
      
      return ray(transformedOrigin, transformedDirection);
   }
};

template<class R>
struct intersect_info
{
   R *object;
   float time;
   
   inline intersect_info<R>()
   {
      object = NULL;
      time = MISS;
   }
   
   inline intersect_info<R>(R *obj, float t)
   {
      object = obj;
      time = t;
   }
};


#endif
