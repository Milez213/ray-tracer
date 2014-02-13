//
//  TestTraceable.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__TestTraceable__
#define __ray_tracer__TestTraceable__

#include <iostream>

#include "Traceable.h"

class TestTraceable : public Traceable
{
public:
   virtual intersect_info<Traceable> Intersect(ray cast);
   virtual bounding_box Bounds();
   
   virtual vec3 Normal(vec3 contact);
   virtual color Shade(vec3 contact);
};

#endif /* defined(__ray_tracer__TestTraceable__) */
