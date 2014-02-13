//
//  Renderable.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_Renderable_h
#define ray_tracer_Renderable_h

#include "glm/glm.hpp"

#include "material.h"

using namespace glm;

class Renderable
{
public:
   virtual vec3 Normal(vec3 contact) = 0;
   virtual color Shade(vec3 contact) = 0;
};


#endif
