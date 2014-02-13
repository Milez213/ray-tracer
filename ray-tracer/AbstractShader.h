//
//  AbstractShader.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__AbstractShader__
#define __ray_tracer__AbstractShader__

#include <iostream>

#include "glm/glm.hpp"
#include "material.h"

#include "AbstractLight.h"

class AbstractShader
{
public:
   static color Shade(vec3 contact, vec3 normal, vec3 cam,
                       const AbstractLight *light);
};

#endif /* defined(__ray_tracer__AbstractShader__) */
