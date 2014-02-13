//
//  main.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include <iostream>

#include "glm/glm.hpp"

#include "Image.h"

#include "Scene.h"
#include "Camera.h"
#include "TestTraceable.h"
#include "AbstractLight.h"

using namespace glm;

int main(int argc, const char * argv[])
{
   int width = 640;
   int height = 480;
   
   vector<Traceable *> objects;
   TestTraceable *tt = new TestTraceable();
   objects.push_back(tt);
   
   vector<AbstractLight *> lights;
   
   Camera *cam = new Camera(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
   cam->SetHeight(height);
   cam->SetWidth(width);
   
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
   
   // insert code here...
   std::cout << "Hello, World!\n";
   return 0;
}

