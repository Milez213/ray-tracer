//
//  PointLight.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__PointLight__
#define __ray_tracer__PointLight__

#include <iostream>

#include "AbstractLight.h"

class PointLight : public AbstractLight
{
public:
   PointLight(color col, vec3 pos);
   
   virtual color Intensity(vec3 contact, vec3 normal) const;
   virtual vec3 GetDirection(vec3 contact) const;
private:
   vec3 position;
   color light;
};

#endif /* defined(__ray_tracer__PointLight__) */
