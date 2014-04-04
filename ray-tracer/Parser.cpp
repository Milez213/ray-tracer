//
//  Parser.cpp
//  ray-tracer
//
//  Created by Bryan Bell on 4/2/14.
//  Copyright (c) 2014 Bryan Bell. All rights reserved.
//

#include "Parser.h"

#define CAM_NAME "turnTableCamera1"

Parser::Parser(const aiScene *scn)
{
   scene = scn;
   
   lights = new vector<AbstractLight *>();
   meshes = new vector<Mesh *>();
   
   aiNode *root = scn->mRootNode;
   meshes = NavigateNode(root);
}

vector<Mesh *> *Parser::NavigateNode(aiNode *node)
{
   vector<Mesh *> *children = new vector<Mesh *>();
   
   int numChildren = node->mNumChildren;
   mat4 *transform = new mat4(node->mTransformation.a1, node->mTransformation.a2,
                              node->mTransformation.a3, node->mTransformation.a4,
                              node->mTransformation.b1, node->mTransformation.b2,
                              node->mTransformation.b3, node->mTransformation.b4,
                              node->mTransformation.c1, node->mTransformation.c2,
                              node->mTransformation.c3, node->mTransformation.c4,
                              node->mTransformation.d1, node->mTransformation.d2,
                              node->mTransformation.d3, node->mTransformation.d4);
   
   /*
    * Loops through the meshes contained in this node and adds them to
    * "triangles".
    */
   for (int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; // The current mesh
      aiVector3D *verts = mesh->mVertices; // The vertices that define the mesh
      aiVector3D *norms = mesh->mNormals;
      vector<Intersectable<Renderable> *> meshTriangles;
      material mat = material();
      for (int j = 0; j < mesh->mNumFaces; j++) {
         aiFace face = mesh->mFaces[j]; // The current face
         if (face.mNumIndices == 3) {
            aiMaterial *mater = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D diff(0.0f, 0.0f, 0.0f);
            aiColor3D spec(0.0f, 0.0f, 0.0f);
            aiColor3D ambt(0.0f, 0.0f, 0.0f);
            float opac;
            aiReturn prop = mater->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
            prop = mater->Get(AI_MATKEY_COLOR_SPECULAR, spec);
            prop = mater->Get(AI_MATKEY_COLOR_AMBIENT, ambt);
            prop = mater->Get(AI_MATKEY_OPACITY, opac);
            mat.diffuse = color(diff.r, diff.g, diff.b);
            mat.specular = color(spec.r, spec.g, spec.b);
            mat.ambient = color(ambt.r, ambt.g, ambt.b);
            mat.opacity = opac;
            
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
                                         mat);
            meshTriangles.push_back(tri);
         }
      }
      Mesh *myMesh = new Mesh(&meshTriangles, mat, *transform);
      children->push_back(myMesh);
   }
   
   /*
    * Loops through the aiCameras contained by "scene" to see if any match the
    * name of the current node.
    */
   for (int i = 0; i < scene->mNumCameras; i++) {
      aiCamera *cam = scene->mCameras[i];
      if (cam->mName == node->mName && cam->mName == aiString(CAM_NAME)) {
         vec3 eye = transform_point(vec3(cam->mPosition.x, cam->mPosition.y, cam->mPosition.z), *transform);
         vec3 lookAt = transform_point(vec3(cam->mLookAt.x, cam->mLookAt.y, cam->mLookAt.z), mat4(1.0f));
         
         printf("Camera: (%0.3f, %0.3f, %0.3f)\n", eye.x, eye.y, eye.z);
         printf("Look At: (%0.3f, %0.3f, %0.3f)\n", lookAt.x, lookAt.y, lookAt.z);
         
         camera = new class Camera(eye,
                              lookAt,
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
   
   if (children->size() == 0)
   {
      children->push_back(new Mesh(*transform));
   }
   
   for (int i = 0; i < numChildren; i++) {
      vector<Mesh *> *subChildren = NavigateNode(node->mChildren[i]);
      children->at(0)->AddChildren(subChildren);
   }
   
   return children;
}