//
//  materials.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef ray_tracer_material_h
#define ray_tracer_material_h

struct color
{
   float r;
   float g;
   float b;
   
   inline color()
   {
      r = 0.0f;
      g = 0.0f;
      b = 0.0f;
   }
   
   inline color(float red, float green, float blue)
   {
      r = red;
      g = green;
      b = blue;
   }
};

struct material
{
   color diffuse;
   color specular;
   color ambient;
   color emissive;
   float reflectivity;
   float refractivity;
   float indexOfRefraction;
   float opacity;
   
   inline material(color diff, color spec, color ambt, color emis,
                   float reflect, float refract,
                   float ior, float opac)
   {
      diffuse = diff;
      specular = spec;
      ambient = ambt;
      emissive = emis;
      reflectivity = reflect;
      refractivity = refract;
      indexOfRefraction = ior;
      opacity = opac;
   }
};



#endif
