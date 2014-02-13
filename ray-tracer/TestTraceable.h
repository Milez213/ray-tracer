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

#include "Intersectable.h"
#include "Renderable.h"

class TestTraceable : public Intersectable<Renderable>, public Renderable
{
public:
   virtual intersect_info<Renderable> Intersect(ray cast);
   virtual bounding_box Bounds();
   
   virtual vec3 Normal(vec3 contact);
   virtual color Shade(vec3 contact, const AbstractLight *light, const Camera *cam);
   virtual material Material() { return mat; };

private:
   material mat;
};

#endif /* defined(__ray_tracer__TestTraceable__) */
