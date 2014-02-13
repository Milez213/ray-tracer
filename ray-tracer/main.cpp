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

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Image.h"

#include "Scene.h"

#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "PointLight.h"
#include "Octree.h"

#include "Renderable.h"

using namespace glm;
using namespace std;

/**
 * \brief This recursive function returns the hierarchy of meshes from the
 *        collada file.
 *
 * \param scene [in] The aiScene given by the collada file
 * \param node [in] The current aiNode in the mesh tree
 * \param triangles [in, out] The vector of RayObjects to be added to and
 *        returned
 * \param lights [in, out] The vector of RayLights to be added to and returned
 * \param camera [out] The RayCamera to be returned
 * \param transforms [in] The current transformation determined by the meshes
 *        position in the hierarchy
 */
void get_node_objects(const aiScene *scene,
                      aiNode *node,
                      vector<Intersectable<Renderable> *> *triangles,
                      vector<AbstractLight *> *lights,
                      Camera **camera,
                      vector<mat4 *> *transforms)
{
   /* The number of children given by the current node */
   int numChildren = node->mNumChildren;
   vector<mat4 *> identity;
   identity.push_back(new mat4(1.0f));
   
   /* The transformation defined by node converted to a mat4 */
   mat4 *transform = new mat4(node->mTransformation.a1, node->mTransformation.a2,
                              node->mTransformation.a3, node->mTransformation.a4,
                              node->mTransformation.b1, node->mTransformation.b2,
                              node->mTransformation.b3, node->mTransformation.b4,
                              node->mTransformation.c1, node->mTransformation.c2,
                              node->mTransformation.c3, node->mTransformation.c4,
                              node->mTransformation.d1, node->mTransformation.d2,
                              node->mTransformation.d3, node->mTransformation.d4);
   
   transforms->push_back(transform);
   
   /*
    * Loops through the meshes contained in this node and adds them to
    * "triangles".
    */
   for (int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; // The current mesh
      aiVector3D *verts = mesh->mVertices; // The vertices that define the mesh
      aiVector3D *norms = mesh->mNormals;
      vector<Intersectable<Renderable> *> meshTriangles;
      for (int j = 0; j < mesh->mNumFaces; j++) {
         aiFace face = mesh->mFaces[j]; // The current face
         if (face.mNumIndices == 3) {
            aiMaterial *mater = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D diff(0.0f, 0.0f, 0.0f);
            aiColor3D spec(0.0f, 0.0f, 0.0f);
            aiColor3D ambt(0.0f, 0.0f, 0.0f);
            aiReturn prop = mater->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
            prop = mater->Get(AI_MATKEY_COLOR_SPECULAR, spec);
            prop = mater->Get(AI_MATKEY_COLOR_AMBIENT, ambt);
            Triangle *tri = new Triangle(vec3(verts[face.mIndices[0]].x,
                                                    verts[face.mIndices[0]].y,
                                                    verts[face.mIndices[0]].z),
                                               vec3(verts[face.mIndices[1]].x,
                                                    verts[face.mIndices[1]].y,
                                                    verts[face.mIndices[1]].z),
                                               vec3(verts[face.mIndices[2]].x,
                                                    verts[face.mIndices[2]].y,
                                                    verts[face.mIndices[2]].z),
                                               vec3(norms[face.mIndices[0]].x,
                                                    norms[face.mIndices[0]].y,
                                                    norms[face.mIndices[0]].z),
                                               vec3(norms[face.mIndices[1]].x,
                                                    norms[face.mIndices[1]].y,
                                                    norms[face.mIndices[1]].z),
                                               vec3(norms[face.mIndices[2]].x,
                                                    norms[face.mIndices[2]].y,
                                                    norms[face.mIndices[2]].z),
                                               material());
            meshTriangles.push_back(tri);
         }
      }
      /*RayMesh *myMesh = new RayMesh(&meshTriangles,
                                    r_makeColor(vec3(0.4f)),
                                    r_makeColor(vec3(0.6f)),
                                    r_makeColor(vec3(0.0f)),
                                    0.0f, 0.0f, 0.0f,
                                    *transforms, phong);
      triangles->push_back(myMesh);*/
   }
   
   /*
    * Loops through the aiCameras contained by "scene" to see if any match the
    * name of the current node.
    */
   for (int i = 0; i < scene->mNumCameras; i++) {
      aiCamera *cam = scene->mCameras[i];
      if (cam->mName == node->mName) {
         vec4 eye = vec4(cam->mPosition.x,
                         cam->mPosition.y,
                         cam->mPosition.z,
                         1.0f) * *transform;
         vec4 lookAt = vec4(cam->mLookAt.x,
                            cam->mLookAt.y,
                            cam->mLookAt.z,
                            1.0f) * *transform;
         
         *camera = new Camera(vec3(eye.x, eye.y, eye.z),
                              vec3(lookAt.x, lookAt.y, lookAt.z),
                              vec3(cam->mUp.x, cam->mUp.y, cam->mUp.z));
      }
   }
   
   /*
    * Loops through the aiLights contained by "scene" to see if any match the
    * name of the current node, adding them to "lights" if so.
    */
   for (int i = 0; i < scene->mNumLights; i++) {
      aiLight *lgt = scene->mLights[i];
      if (lgt->mName == node->mName) {
         vec4 pos = vec4(lgt->mPosition.x,
                         lgt->mPosition.y,
                         lgt->mPosition.z,
                         1.0f) * *transform;
         color col = color(lgt->mColorDiffuse.r,
                                        lgt->mColorDiffuse.g,
                                        lgt->mColorDiffuse.b);
         
         PointLight *light = new PointLight(col, vec3(pos.x, pos.y, pos.z));
         lights->push_back(light);
      }
   }
   
   for (int i = 0; i < numChildren; i++) {
      get_node_objects(scene, node->mChildren[i], triangles, lights, camera,
                       transforms);
   }
}

/**
 * \brief This function takes an aiScene and returns the meshes, lights, and
 *        camera contained by it.
 *
 * \param scene [in] The aiScene to be parsed
 * \param triangles [out] The meshes contained by this scene
 * \param lights [out] The lights contained by this scene
 * \param camera [out] The camera contained by this scene
 */
int get_assimp_objects(const aiScene *scene,
                       vector<Intersectable<Renderable> *> *triangles,
                       vector<AbstractLight *> *lights,
                       Camera **camera)
{
   aiNode *node = scene->mRootNode; //The root node
   int numChildren = node->mNumChildren; //The number of children contained by
   //the root node
   
   vector<mat4 *> transforms; //The stack of transformations built while
   //traversing the hierarchy
   transforms.push_back(new mat4(1.0f)); //Initializing the stack with just the
   //identity matrix
   
   for (int i = 0; i < numChildren; i++) {
      get_node_objects(scene, node->mChildren[i], triangles, lights, camera,
                       &transforms);
   }
   
   return 0;
}

vector<Intersectable<Renderable> *> *TestTriangles()
{
   vector<Intersectable<Renderable> *> *triangles = new vector<Intersectable<Renderable> *>();
   Triangle *tri = new Triangle(vec3(1.0f, 1.0f, 0.0f),
                                vec3(1.0f, -1.0f, 0.0f),
                                vec3(-1.0f, 1.0f, 0.0f),
                                vec3(0.0f, 0.0f, 1.0f),
                                vec3(0.0f, 0.0f, 1.0f),
                                vec3(0.0f, 0.0f, 1.0f), material());
   triangles->push_back(tri);
   
   return triangles;
}

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
   
   vector<Intersectable<Renderable> *> *objects = TestTriangles();
   
   Octree *oct = new Octree(objects);
   vector<Intersectable<Renderable> *> octrees;
   octrees.push_back(oct);
   
   vector<AbstractLight *> lights;
   lights.push_back(new PointLight(color(0.8f), vec3(5.0f)));
   lights.push_back(new PointLight(color(0.9f),
                                   vec3(1.0f, 1.0f, 7.0f)));
   
   Camera *cam = new Camera(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
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

