//
//  PhongShader.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "PhongShader.h"

PhongShader *PhongShader::instance = NULL;

PhongShader *PhongShader::GetInstance()
{
   if (!PhongShader::instance)
   {
      PhongShader::instance = new PhongShader();
   }
   
   return PhongShader::instance;
}

color PhongShader::Shade(vec3 contact, vec3 normal, vec3 cam, material mat, const AbstractLight *light)
{
   color shade;
   
	vec3 lightDirection = light->GetDirection(contact);
   
	float dirNor = dot(lightDirection, normal);
   if (dirNor < 0.0f)
   {
      dirNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * mat.diffuse * dirNor;
	
	vec3 cameraDirection = normalize(cam - contact);
	vec3 difference = normalize(lightDirection + cameraDirection);
	
	float difNor = dot(difference, normal);
   if (difNor < 0.0f)
   {
      difNor = 0.0f;
   }
	
	shade += light->Intensity(contact, normal) * pow(difNor, 1.0f/mat.roughness) * mat.specular;
   
	return shade;
   
}