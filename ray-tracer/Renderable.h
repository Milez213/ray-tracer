//
//  Renderable.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_Renderable_h
#define ray_tracer_Renderable_h

#include "glm/glm.hpp"

#include "AbstractLight.h"
#include "Camera.h"

#include "material.h"

using namespace glm;

class Renderable
{
public:
   /**
    * \brief Returns the normal at the given contact point
    *
    * \return The normal at the given contact point
    */
   virtual vec3 Normal(vec3 contact) = 0;
   
   /**
    * \brief Calculates the color at the given point using the position of the
    *        camera and the light
    *
    * \param contact [in] The point on the object used to calculate the color
    * \param cam [in] The position of the camera used in specular highlight
    *        calculations
    * \param light [in] The light being used to calculate the color
    * \return The color at the given point on the Renderable
    */
   virtual color Shade(vec3 contact, vec3 cam, const AbstractLight *light) = 0;
   
   /**
    * \brief Returns the material of the renderable
    *
    * \return The material of the renderable
    */
   virtual material Material() = 0;
};


#endif
