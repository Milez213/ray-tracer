//
//  TestTraceable.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "TestTraceable.h"


intersect_info<Renderable> TestTraceable::Intersect(ray cast)
{
   return intersect_info<Renderable>(this, 0.0f);
}

bounding_box TestTraceable::Bounds()
{
   return bounding_box(vec3(0.0f), vec3(1.0f));
}

vec3 TestTraceable::Normal(vec3 contact)
{
   return vec3(0.0f, 1.0f, 0.0f);
}

color TestTraceable::Shade(vec3 contact, const AbstractLight *light, const Camera *cam)
{
   return color(1.0f, 1.0f, 1.0f);
}