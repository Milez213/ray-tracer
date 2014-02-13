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

Scene::Scene(const vector<Traceable *> *objcts,
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

Scene *Scene::CreateScene(const vector<Traceable *> *objects,
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
   
   for (int i = 0; i < camera->Width(); i++)
   {
      pixels[i] = (color *)malloc(sizeof(color) * camera->Height());
      for (int j = 0; j < camera->Height(); j++) {
         pixels[i][j] = color((float)i/(float)camera->Width(),
                              (float)j/(float)camera->Height(),
                              0.0f);
      }
   }
   
   return pixels;
}