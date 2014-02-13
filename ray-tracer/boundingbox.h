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
   
   inline bounding_box(vec3 min, vec3(max))
   {
      minimum = min;
      maximum = max;
   }
};

#endif
