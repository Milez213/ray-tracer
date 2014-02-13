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

vector<Intersectable<Renderable> *> *TestScene()
{
   vector<Intersectable<Renderable> *> *objects = new vector<Intersectable<Renderable> *>();
   Sphere *s;
   float width = 70.0f;
   float height = 50.0f;
   float depth = 50.0f;
   float x;
   float y;
   float z;
   
   srand((unsigned int)time(0));
   
   for (int i = 0; i < 100; i++)
   {
      x = (float)(rand() % (int)(width * 100) - (int)(width * 100 / 2)) * 0.01f;
      for (int j = 0; j < 100; j++)
      {
         y = (float)(rand() % (int)(height * 100) - (int)(height * 100 / 2)) * 0.01f;
         for (int k = 0; k < 100; k++)
         {
            z = (float)(rand() % (int)(depth * 100) - (int)(depth * 100 / 2)) * 0.01f;
            s = new Sphere(vec3(x, y, z), 0.5f, material());
            objects->push_back(s);
         }
      }
   }
   
   return objects;
}

int main(int argc, const char * argv[])
{
   int width = 1280;
   int height = 720;
   
   vector<Intersectable<Renderable> *> *objects = TestScene();
   
   Octree *oct = new Octree(objects);
   vector<Intersectable<Renderable> *> octrees;
   octrees.push_back(oct);
   
   vector<AbstractLight *> lights;
   lights.push_back(new PointLight(color(0.7f), vec3(5.0f)));
   lights.push_back(new PointLight(color(0.5f),
                                   vec3(-5.0f, 5.0f, 1.0f)));
   
   Camera *cam = new Camera(vec3(0.0f, 0.0f, 50.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
   cam->SetHeight(height);
   cam->SetWidth(width);
   cam->SetAntiAliasing(1);
   
   Scene *scene = Scene::CreateScene(&octrees, &lights, cam);
   
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

