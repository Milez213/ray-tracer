//
//  main.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include <iostream>
#include <vector>

#include "glm/glm.hpp"

#include "Image.h"

#include "Scene.h"

#include "Camera.h"
#include "Sphere.h"
#include "PointLight.h"
#include "Octree.h"

#include "Renderable.h"

using namespace glm;
using namespace std;

int main(int argc, const char * argv[])
{
   int width = 1920;
   int height = 1080;
   
   vector<Intersectable<Renderable> *> objects;
   Sphere *s = new Sphere(vec3(0.0f), 1.0f, material());
   s = new Sphere(vec3(2.0f), 1.0f, material());
   objects.push_back(s);
   Octree *oct = new Octree(&objects);
   /*vector<Intersectable<Renderable> *> octrees;
   octrees.push_back(oct);*/
   
   vector<AbstractLight *> lights;
   lights.push_back(new PointLight(color(0.7f), vec3(5.0f)));
   lights.push_back(new PointLight(color(0.5f),
                                   vec3(-5.0f, 5.0f, 1.0f)));
   
   Camera *cam = new Camera(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
   cam->SetHeight(height);
   cam->SetWidth(width);
   cam->SetAntiAliasing(3);
   
   Scene *scene = Scene::CreateScene(&objects, &lights, cam);
   
   color ** image = scene->TraceScene();
   
   Image *output = new Image(width, height);
   
   for (int i = 0; i < width; i++)
   {
      for (int j = 0; j < height; j++)
      {
         color_t pxl = color_t(image[i][j].r, image[i][j].g, image[i][j].b, 1.0f);
         output->pixel(i, j, pxl);
      }
   }
   
   output->WriteTga("output.tga");
   return 0;
}

