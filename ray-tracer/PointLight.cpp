//
//  PointLight.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "PointLight.h"

PointLight::PointLight(color col, vec3 pos)
{
   position = pos;
   light = col;
}

color PointLight::Intensity(vec3 contact, vec3 normal) const
{
   return light;
}

vec3 PointLight::GetDirection(vec3 contact) const
{
   return normalize(position - contact);
}