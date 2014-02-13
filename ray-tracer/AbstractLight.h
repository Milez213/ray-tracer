//
//  AbstractLight.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__AbstractLight__
#define __ray_tracer__AbstractLight__


#include <iostream>

#include "glm/glm.hpp"

#include "material.h"

using namespace glm;

class AbstractLight
{
public:
   virtual color Intensity(vec3 contact, vec3 normal) const = 0;
   virtual vec3 GetDirection(vec3 contact) const = 0;
};



#endif /* defined(__ray_tracer__AbstractLight__) */
