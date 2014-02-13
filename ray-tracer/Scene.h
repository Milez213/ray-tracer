//
//  Scene.h
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Scene__
#define __ray_tracer__Scene__

#include <iostream>
#include <vector>

#include "material.h"

#include "AbstractLight.h"
#include "Intersectable.h"
#include "Renderable.h"
#include "Camera.h"

#define INF 10000.0f

using namespace std;

class Scene
{
public:
   static Scene *CreateScene(const vector<Intersectable<Renderable> *> *objects,
                             const vector<AbstractLight *> *lights,
                             const Camera *camera);
   static Scene *GetScene();
   
   color **TraceScene();
   
   color BackgroundColor() { return background; };
   void SetBackgroundColor(color bg) { background = bg; };
   int IndirectLightingBounces() { return indirectLightingBounces; };
   void SetIndirectLightingBounces(int bounces) { indirectLightingBounces = bounces; };
   int IndirectLightingRays() { return indirectRays; };
   void SetIndirectLightingRays(int rays) { indirectRays = rays; };
   
   const vector<Intersectable<Renderable> *> *Objects() { return objects; };
   
private:
   Scene(const vector<Intersectable<Renderable> *> *objects,
         const vector<AbstractLight *> *lights,
         const Camera *camera);
   
   color TraceRay(ray cast, int depth);
   color CalculateColor(ray cast, vec3 contact, Renderable *object, int depth);
   color CastIndirectRays(vec3 contact, vec3 normal, int depth);
   
   float IsPointShadowed(vec3 contact, AbstractLight *light);
   
   static Scene *instance;
   
   const vector<AbstractLight *> *lights;
   const vector<Intersectable<Renderable> *> *objects;
   const Camera *camera;
   
   color background;
   
   int indirectLightingBounces;
   int indirectRays;
};

#endif /* defined(__ray_tracer__Scene__) */
