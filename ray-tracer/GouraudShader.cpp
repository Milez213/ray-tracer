//
//  PhongShader.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "GouraudShader.h"

GouraudShader *GouraudShader::instance = NULL;

GouraudShader *GouraudShader::GetInstance()
{
   if (!GouraudShader::instance)
   {
      GouraudShader::instance = new GouraudShader();
   }
   
   return GouraudShader::instance;
}

color GouraudShader::Shade(vec3 contact, vec3 normal, vec3 cam, material mat, const AbstractLight *light)
{
   color shade;
   
	vec3 lightDirection = light->GetDirection(contact);
   
	float dirNor = dot(lightDirection, normal);
   if (dirNor < 0.0f)
   {
      dirNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * mat.diffuse * dirNor;
   
	return shade;
   
}