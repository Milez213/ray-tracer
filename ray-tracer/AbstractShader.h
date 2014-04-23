//
//  AbstractShader.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/13/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__AbstractShader__
#define __ray_tracer__AbstractShader__

#include <iostream>

#include "glm/glm.hpp"
#include "material.h"

#include "AbstractLight.h"

class AbstractShader
{
public:
   /**
    * \brief Shades a point with a given normal using the given light, camera,
    *        and material
    *
    * \param contact [in] The point being shaded
    * \param normal [in] The normal of the object being shaded at that point
    * \param cam [in] The position of the camera to be used when calculating
    *        specular highlights
    * \param mat [in] The material of the object being shaded
    * \param light [in] The light being used when calculating the shading
    * \return The color at the given point
    */
   virtual color Shade(vec3 contact, vec3 normal, vec3 cam,
                       material mat, const AbstractLight *light) = 0;
};

#endif /* defined(__ray_tracer__AbstractShader__) */
