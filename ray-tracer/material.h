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
   
   inline color(float grayscale)
   {
      r = grayscale;
      g = grayscale;
      b = grayscale;
   }
   
   inline color(float red, float green, float blue)
   {
      r = red;
      g = green;
      b = blue;
   }
   
   inline color operator+(color col)
   {
      return color(col.r + r, col.g + g, col.b + b);
   }
   
   inline color operator*(color col)
   {
      return color(col.r * r, col.g * g, col.b * b);
   }
   
   inline color& operator+=(color col)
   {
      this->r += col.r;
      this->g += col.g;
      this->b += col.b;
      
      return *this;
   }
   
   inline color operator*(float modify)
   {
      return color(modify * r, modify * g, modify * b);
   }
   
   inline color& operator/=(float modify)
   {
      this->r /= modify;
      this->g /= modify;
      this->b /= modify;
      
      return *this;
   }
};

struct material
{
   color diffuse;
   color specular;
   color ambient;
   color emissive;
   float roughness;
   float reflectivity;
   float refractivity;
   float indexOfRefraction;
   float opacity;
   
   inline material()
   {
      diffuse = color(0.0f, 0.3f, 0.0f);
      specular = color();
      ambient = color();
      emissive = color();
      roughness = 0.1f; // Cannot be zero
      reflectivity = 0.0f;
      refractivity = 0.0f;
      indexOfRefraction = 1.0f;
      opacity = 1.0f;
   }
   
   inline material(color diff, color spec, color ambt, color emis,
                   float rough, float reflect, float refract,
                   float ior, float opac)
   {
      diffuse = diff;
      specular = spec;
      ambient = ambt;
      emissive = emis;
      roughness = rough;
      reflectivity = reflect;
      refractivity = refract;
      indexOfRefraction = ior;
      opacity = opac;
   }
};



#endif
