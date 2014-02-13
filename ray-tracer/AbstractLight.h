//
//  AbstractLight.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__AbstractLight__
#define __ray_tracer__AbstractLight__

class Scene;

#include <iostream>

#include "glm/glm.hpp"

#include "material.h"
#include "Scene.h"

using namespace glm;

class AbstractLight
{
public:
   AbstractLight() {};
   virtual color Intensity(vec3 contact, vec3 normal, Scene *scene) = 0;
};



#endif /* defined(__ray_tracer__AbstractLight__) */
