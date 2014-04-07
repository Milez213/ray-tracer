//
//  Parser.h
//  ray-tracer
//
//  Created by Bryan Bell on 4/2/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#ifndef __ray_tracer__Parser__
#define __ray_tracer__Parser__

#include <iostream>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"

#include "PointLight.h"

#include "Camera.h"

class Parser
{
public:
   Parser(const aiScene *scene);
   
   void UpdateScene(float time);
   
   vector<Intersectable<Renderable> *> *Meshes() { return (vector<Intersectable<Renderable> *> *)meshes; };
   vector<AbstractLight *> *Lights() { return lights; };
   Camera *Camera() { return camera; };
   
private:
   vector<Mesh *> *NavigateNode(aiNode *node, float time);
   
   const aiScene *scene;
   
   vector<Mesh *> *meshes;
   vector<AbstractLight *> *lights;
   class Camera *camera;
};

#endif /* defined(__ray_tracer__Parser__) */
