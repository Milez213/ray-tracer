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
   lights = lghts;
   objects = objcts;
   camera = cam;
   
   background = color(0.0f, 0.0f, 0.0f);
   indirectLightingBounces = 0;
   indirectRays = 256;
}


#pragma mark Singleton Accessors

Scene *Scene::CreateScene(const vector<Intersectable<Renderable> *> *objects,
                          const vector<AbstractLight *> *lights,
                          const Camera *camera)
{
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
   color **pixels = (color **)malloc(sizeof(color *) * camera->Width());
   
   ray ***cast = camera->CastRays();
   int antialiasing = powf(camera->AntiAliasing(), 2);
   
   for (int i = 0; i < camera->Width(); i++)
   {
      pixels[i] = (color *)malloc(sizeof(color) * camera->Height());
      for (int j = 0; j < camera->Height(); j++) {
         pixels[i][j] = color();
         for (int k = 0; k < antialiasing; k++) {
            pixels[i][j] += TraceRay(cast[i][j][k], 0);
         }
         pixels[i][j] /= antialiasing;
      }
   }
   
   return pixels;
}

color Scene::TraceRay(ray cast, int depth)
{
   color collected = background;
   
   Renderable *object;
   float time = INF;
   
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, 0.0f);
   
   for (int i = 0; i < objects->size(); i++)
   {
      info = objects->at(i)->Intersect(cast);
      
      if (info.time != MISS && info.time < time)
      {
         time = info.time;
         object = info.object;
      }
   }
   
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
   color calculated = color();
   
   float reflectedRatio = object->Material().reflectivity;
   float refractedRatio = object->Material().refractivity;
   float shadedRatio = 1.0f - (reflectedRatio + refractedRatio);
   
   AbstractLight *light;
   float shadowed;
   
   vec3 shadowPoint;
   vec3 normal;
   
   ray reflected;
   ray refracted;
   
   for (int i = 0; i < lights->size(); i++)
   {
      light = lights->at(i);
      normal = object->Normal(contact);
      shadowPoint = contact + (normal * STEP);
      
      shadowed = IsPointShadowed(shadowPoint, light);
      
      if (shadedRatio > 0.0f && depth <= indirectLightingBounces) {
         calculated += (object->Shade(contact, camera->Position(), light) + CastIndirectRays(contact, normal, depth)) * shadedRatio * shadowed;
      }
      if (reflectedRatio > 0.0f && depth <= indirectLightingBounces) {
         reflected = cast.reflect_ray(contact, normal);
         calculated += TraceRay(reflected, depth+1);
      }
      if (refractedRatio > 0.0f && depth <= indirectLightingBounces) {
         refracted = cast.refract_ray(contact, normal, object->Material().indexOfRefraction, 1.0f); // FIX THIS!!!
         calculated += TraceRay(refracted, depth+1);
      }
   }
   
   calculated += object->Material().ambient + object->Material().emissive;
   
   return calculated;
}

float Scene::IsPointShadowed(vec3 contact, AbstractLight *light)
{
   float shadowed = 0.0f;
   
   Renderable *object = NULL;
   float time;
   
   ray cast = ray(contact, light->GetDirection(contact));
   
   intersect_info<Renderable> info = intersect_info<Renderable>(NULL, 0.0f);
   
   do {
      time = INF;
      
      for (int i = 0; i < objects->size(); i++)
      {
         info = objects->at(i)->Intersect(cast);
         
         if (info.time != MISS && info.time < time)
         {
            time = info.time;
            object = info.object;
         }
      }
      
      if (time != INF)
      {
         shadowed += object->Material().opacity;
      }
      
      cast = ray(contact + (cast.direction * time), cast.direction);
   } while (time != INF && shadowed < 0.0f);
   
   return shadowed;
}