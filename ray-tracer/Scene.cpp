//
//  Scene.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Scene.h"

Scene *Scene::instance = NULL;

#pragma mark Private Constructor

Scene::Scene(const vector<Intersectable<Renderable> *> *objcts,
             const vector<AbstractLight *> *lghts,
             const Camera *cam)
{
   lights = lghts; // Set the lights for the scene
   objects = objcts; // Set the objects for the scene
   camera = cam; // Set the camera for the scene
   
   background = color(0.0f, 0.0f, 0.0f); // Set the background color to black
   indirectLightingBounces = 0; // The default number of allowed indirect bounces allowed is 0
   indirectRays = 256; // The default number of indirect rays cast is 256
}


#pragma mark Singleton Accessors

Scene *Scene::CreateScene(const vector<Intersectable<Renderable> *> *objects,
                          const vector<AbstractLight *> *lights,
                          const Camera *camera)
{
   /* If the instance of the scene hasn't been created, create it */
   /* Otherwise, just return the existing scene, parameters are ignored */
   if (Scene::instance == NULL)
   {
      Scene::instance = new Scene(objects, lights, camera);
   }
   
   return Scene::instance;
}

Scene *Scene::GetScene()
{
   return instance;
}


#pragma mark Trace Scene

color **Scene::TraceScene()
{
   color **pixels = (color **)malloc(sizeof(color *) * camera->Width()); // The array of colors describing the rendered image
   
   ray ***cast = camera->CastRays(); // The rays cast from the camera
   int antialiasing = powf(camera->AntiAliasing(), 2); // The number of rays cast to calulate anti-aliasing
   
   int totalRays = camera->Width() * camera->Height() * antialiasing; // The total number of rays
   int percent; // The percent of rays which have been rendered
   int lastPercent = -1; // Used to determine if the percent has changed
   
   /* For each column of pixels across the image, determine the colors */
   for (int i = 0; i < camera->Width(); i++)
   {
      pixels[i] = (color *)malloc(sizeof(color) * camera->Height());
      
      /* For each pixel in the column, calculate its color */
      for (int j = 0; j < camera->Height(); j++) {
         pixels[i][j] = color();
         
         /* For each anti-aliasing ray given for the pixel, calculate the color */
         for (int k = 0; k < antialiasing; k++) {
            percent = (int)(((float)(i * camera->Height() * antialiasing + j * antialiasing + k) / (float)totalRays) * 100);
            
            /* If the percent (no decimals) of ray colors calculated has changed
             * since the last ray color calculated, display it to the user */
            if (percent != lastPercent)
            {
               printf("%d%% ", percent);
               fflush(stdout);
               lastPercent = percent;
            }
            pixels[i][j] += TraceRay(cast[i][j][k], 0); // Calculate the color for the given ray
         }
         pixels[i][j] /= antialiasing;
      }
   }
   
   /* Clean up the rays allocated by the camera */
   camera->CleanUpRays(cast);
   
   return pixels;
}

color Scene::TraceRay(ray cast, int depth)
{
   color collected = background; // Set the return color to the background color to start
   
   Renderable *object;
   float time = INF;
   
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, 0.0f);
   
   /* For each object in the scene, test it for interection with the ray */
   for (int i = 0; i < objects->size(); i++)
   {
      info = objects->at(i)->Intersect(cast);
      
      /* If the ray hits the object, and it hasn't already hit a closer object,
       * use that object to calculate the color */
      if (info.time != MISS && info.time < time)
      {
         time = info.time;
         object = info.object;
      }
   }
   
   /* If an object was hit, use it to calculate the color,
    * otherwise return the background color */
   if (time != INF) {
      collected = CalculateColor(cast, cast.origin + (cast.direction * time), object, depth);
   }
   
   return collected;
}

color Scene::CastIndirectRays(vec3 contact, vec3 normal, int depth)
{
   return color(0.0f, 0.0f, 0.0f);
}

color Scene::CalculateColor(ray cast, vec3 contact, Renderable *object, int depth)
{
   color calculated = color(); // The color to be returned
   
   float reflectedRatio = object->Material().reflectivity; // The percent of the color which should be determined from reflected color
   float refractedRatio = object->Material().refractivity; // The percent of the color which should be determined from refracted color
   float shadedRatio = 1.0f - (reflectedRatio + refractedRatio); // The percent of the color which should be determined by shading the object
   
   AbstractLight *light; // The temporary holder for the light to be used to calculate the shadowing and color
   float shadowed; // The percentage which the given point which is obscured from light sources
   
   vec3 shadowPoint; // The adjusted point to be used when determining the percentage shadowed
   vec3 normal; // The normal at the given point
   
   ray reflected; // The reflected ray from the point
   ray refracted; // The refracted ray from the point
   
   for (int i = 0; i < lights->size(); i++)
   {
      light = lights->at(i); // The light to be tested for shadowing
      normal = object->Normal(contact); // The normal at the given point
      shadowPoint = contact + (normal * STEP); // The point adjusted to avoid self shadowing
      
      shadowed = IsPointShadowed(shadowPoint, light); // Determine how much the point is in shadow
      
      /* If the color is determined by shading the object, and we haven't 
       * reached the maximum number of indirect lighting bounces, proceed */
      if (shadedRatio > 0.0f && depth <= indirectLightingBounces) {
         calculated += (object->Shade(contact, camera->Position(), light) + CastIndirectRays(contact, normal, depth)) * shadedRatio * shadowed;
      }
      
      /* If the color is determined by reflected color, and we haven't
       * reached the maximum number of indirect lighting bounces, proceed */
      if (reflectedRatio > 0.0f && depth <= indirectLightingBounces) {
         reflected = cast.reflect_ray(contact, normal);
         calculated += TraceRay(reflected, depth+1);
      }
      
      /* If the color is determined by refracted color, and we haven't
       * reached the maximum number of indirect lighting bounces, proceed */
      if (refractedRatio > 0.0f && depth <= indirectLightingBounces) {
         refracted = cast.refract_ray(contact, normal, object->Material().indexOfRefraction, 1.0f); // FIX THIS!!!
         calculated += TraceRay(refracted, depth+1);
      }
   }
   
   /* Add in ambient and emmissive color */
   calculated += object->Material().ambient + object->Material().emissive;
   
   return calculated;
}

float Scene::IsPointShadowed(vec3 contact, AbstractLight *light)
{
   float shadowed = 1.0f; // Start at 100% unobscured
   
   Renderable *object = NULL;
   float time;
   
   ray cast = ray(contact, light->GetDirection(contact));
   
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, MISS);
   
   /* While the light has not been reached by the ray, and the point is still
    * unobscured, see if there are objects in the way */
   do {
      time = INF;
      
      /* For each object, see if it obscures the point */
      for (int i = 0; i < objects->size(); i++)
      {
         info = objects->at(i)->SafeIntersect(cast);
         
         /* If the object obscures the point calculate how much it obscures the 
          * point */
         if (info.time > 0.0f && info.time < time)
         {
            time = info.time;
            object = info.object;
         }
      }
      
      /* If an object was hit, calculate how much it obscures the point */
      if (time != INF)
      {
         shadowed *= (1.0f - object->Material().opacity);
      }
      
      cast = ray(cast.origin + (cast.direction * (time + STEP)), cast.direction);
   } while (time != INF && shadowed > 0.01f);
   
   return shadowed;
}