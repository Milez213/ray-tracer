//
//  Intersectable.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_Intersectable_h
#define ray_tracer_Intersectable_h

#include "ray.h"
#include "boundingbox.h"

template<class T>
class Intersectable
{
public:
   virtual intersect_info<T> Intersect(ray cast) = 0;
   virtual bounding_box Bounds() = 0;
};

#endif
