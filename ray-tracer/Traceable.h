//
//  Traceable.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_Traceable_h
#define ray_tracer_Traceable_h

#include "Intersectable.h"
#include "Renderable.h"

class Traceable : public Intersectable<Traceable> , public Renderable
{
   virtual intersect_info<Traceable> Intersect(ray cast) = 0;
   virtual bounding_box Bounds() = 0;
   
   virtual vec3 Normal(vec3 contact) = 0;
   virtual color Shade(vec3 contact) = 0;
};

#endif
