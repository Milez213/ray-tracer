//
//  PhongShader.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__PhongShader__
#define __ray_tracer__PhongShader__

#include <iostream>

#include "AbstractShader.h"

class PhongShader : public AbstractShader
{
public:
   static PhongShader *GetInstance();
   virtual color Shade(vec3 contact, vec3 normal, vec3 cam,
                       material mat, const AbstractLight *light);
   
private:
   PhongShader() {};
   
   static PhongShader *instance;
};

#endif /* defined(__ray_tracer__PhongShader__) */
