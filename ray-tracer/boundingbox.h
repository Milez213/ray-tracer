//
//  boundingbox.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_boundingbox_h
#define ray_tracer_boundingbox_h

#include "glm/glm.hpp"

using namespace glm;

struct bounding_box
{
   vec3 minimum;
   vec3 maximum;
   
   inline bounding_box()
   {
      minimum = vec3(0.0f);
      maximum = vec3(0.0f);
   }
   
   inline bounding_box(vec3 min, vec3 max)
   {
      minimum = min;
      maximum = max;
   }
   
   inline bool intersects(bounding_box bounds)
   {
      if (maximum.x < bounds.minimum.x) { return false; }
      if (minimum.x > bounds.maximum.x) { return false; }
      if (maximum.y < bounds.minimum.y) { return false; }
      if (minimum.y > bounds.maximum.y) { return false; }
      if (maximum.z < bounds.minimum.z) { return false; }
      if (minimum.z > bounds.maximum.z) { return false; }
      
      return true;
   }
};

#endif
