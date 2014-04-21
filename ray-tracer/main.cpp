//
//  main.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 2/12/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include <iostream>
#include <vector>

#include "Parser.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include "Image.h"

#include "Scene.h"

#include "Camera.h"
#include "AbstractLight.h"
#include "Renderable.h"
#include "Intersectable.h"

#define NUM_FRAMES 70
#define FRAMES_PER_SECOND 24

#define FRAME_PREFIX "output"
#define FRAME_EXTENSION ".tga"

using namespace std;

int main(int argc, const char * argv[])
{
   int width = 1920;
   int height = 1080;
   
   Camera *camera;
   char *frameName = (char *)calloc(sizeof(char), string(FRAME_PREFIX).length() + 4 + string(FRAME_EXTENSION).length());
   
   Assimp::Importer importer;
   const aiScene *aScene = importer.ReadFile("ART-474/scenes/collada/bouncingBall.dae",
                                            aiProcess_Triangulate);
   
   Parser *parser = new Parser(aScene);
   camera = parser->Camera();
   
   Scene *scene = Scene::CreateScene(parser->Meshes(), parser->Lights(), camera);
   scene->SetBackgroundColor(color(1.0f, 1.0f, 1.0f));
   
   Image *output = new Image(width, height);
   
   for (int i = 0; i < NUM_FRAMES; i++)
   {
      cout << "Starting Frame " << i << endl;
      
      parser->UpdateScene((float)i / (float)FRAMES_PER_SECOND);
      camera->SetWidth(width);
      camera->SetHeight(height);
      camera->SetAntiAliasing(2);
      
      sprintf(frameName, "%s%03d%s", FRAME_PREFIX, i, FRAME_EXTENSION);
      color **image = scene->TraceScene();
      
      for (int i = 0; i < width; i++)
      {
         for (int j = 0; j < height; j++)
         {
            color_t pxl = color_t(image[i][j].r, image[i][j].g, image[i][j].b, 1.0f);
            output->pixel(i, j, pxl);
         }
      }
      
      output->WriteTga(frameName);
      
      cout << "100%" << endl;
   }
   
   return 0;
}
 
